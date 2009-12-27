/*
Datentype ausgeben

$Copyright (C) 1994 Erich Frühstück
This file is part of EFEU.

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Library General Public
License as published by the Free Software Foundation; either
version 2 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty
of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU Library General Public License for more details.

You should have received a copy of the GNU Library General Public
License along with this library; see the file COPYING.Library.
If not, write to the Free Software Foundation, Inc.,
59 Temple Place, Suite 330, Boston, MA 02111-1307, USA.
*/

#include <EFEU/object.h>
#include <EFEU/printobj.h>
#include <EFEU/stdtype.h>
#include <EFEU/EfiPar.h>
#include <ctype.h>

static int PrintTypeDepth = 0;

static int not_a_name (const char *name)
{
	if	(name == NULL)	return 1;

	if	(!(*name == '_' || isalpha((unsigned char) *name)))
		return 1;

	for (; *name; name++)
		if (!(*name == '_' || isalnum((unsigned char) *name)))
			return 1;

	return 0;
}

static int show_version (IO *io, const EfiType *type, int space)
{
	EfiControl *ctrl = GetEfiPar((EfiType *) type,
		&EfiPar_control, "version");

	if	(ctrl && ctrl->data)
	{
		int n = io_nputc(' ', io, space);
		return n + io_printf(io, "<%s>", ctrl->data);
	}

	return 0;
}

static int show_enum (IO *io, const EfiType *type, int verbosity)
{
	int n;
	char delim;
	int depth;

	n = io_puts("enum", io);

	if	(type->name && type->name[0] != '_')
	{
		n += io_nputc(' ', io, 1);
		n += io_puts(type->name, io);
	}

	if	(verbosity > 1)
	{
		delim = '\n';
		depth = PrintTypeDepth + 1;
	}
	else
	{
		delim = ' ';
		depth = 0;
	}

	if	(type->base && type->base != &Type_enum)
	{
		n += io_puts(" : ", io);
		n += PrintType(io, type->base, verbosity);
	}
	else if	(type->recl != 4)
	{
		n += io_printf(io, " : %d", type->recl);
	}

	n += io_puts(" {", io);

	if	(type->vtab)
	{
		VarTabEntry *p;
		size_t k;

		p = type->vtab->tab.data;
		k = type->vtab->tab.used;

		for (; k-- > 0; p++)
		{
			if	(p->obj && p->obj->type == type)
			{
				n += io_nputc(delim, io, 1);
				n += io_nputc('\t', io, depth);

				if	(not_a_name(p->name))
				{
					n += io_puts("\"", io);
					n += io_xputs(p->name, io, "\"");
					n += io_puts("\"", io);
				}
				else	n += io_puts(p->name, io);

				n += io_printf(io, " = %d,",
					Val_int(p->obj->data));

				if	(p->desc && verbosity > 1)
					n += io_printf(io, " /* %s */",
						p->desc);
			}
		}
	}

	if	(verbosity > 1)
	{
		n += io_nputc('\n', io, 1);
		n += io_nputc('\t', io, PrintTypeDepth);
	}
	else	n += io_nputc(' ', io, 1);

	n += io_nputc('}', io, 1);
	return n;
}

static int show_name (IO *io, EfiStruct *st)
{
	int n = io_nputc(' ', io, 1);
	n += io_puts(st->name, io);

	if	(st->dim > 1)
		n += io_printf(io, "[%d]", st->dim);

	return n;
}

static int show_struct (IO *io, const EfiType *type, int verbosity)
{
	EfiStruct *st;
	EfiType *last;
	char *desc;
	int n;

	n = io_puts("struct", io);

	if	(type->name && type->name[0] != '_')
	{
		n += io_nputc(' ', io, 1);
		n += io_puts(type->name, io);
		n += show_version(io, type, 1);
	}

	st = type->list;

	if	(type->base)
	{
		n += io_puts(" : ", io);
		n += PrintType(io, type->base, verbosity);

		if	(st && st->type == type->base && st->offset == 0)
		{
			if	(st->name)
				n += show_name(io, st);
			st = st->next;
		}
	}

	n += io_puts(" {", io);
	PrintTypeDepth++;

	if	(verbosity > 1)
		n += io_puts("\n", io);

	last = NULL;
	desc = NULL;

	for (; st != NULL; st = st->next)
	{
		if	(!st->name)	continue;

		if	(verbosity > 1 || st->type != last)
		{
			if	(last != NULL)
			{
				n += io_puts(";", io);

				if	(desc && verbosity > 1)
				{
					n += io_puts(" /* ", io);
					n += io_puts(desc, io);
					n += io_puts(" */", io);
				}

				if	(verbosity > 1)
					n += io_puts("\n", io);
			}

			if	(st->type == type->base)
			{
				if	(verbosity > 1)
					n += io_nputc('\t', io, PrintTypeDepth);
				else	n += io_nputc(' ', io, 1);

				n += io_puts(st->type->name, io);
			}
			else
			{
				if	(verbosity <= 1)
					n += io_nputc(' ', io, 1);

				n += PrintType(io, st->type, verbosity);
			}
		}
		else	n += io_nputc(',', io, 1);

		n += show_name(io, st);
		last = st->type;

		if	(verbosity > 1)
			desc = st->desc;
	}

	PrintTypeDepth--;

	if	(verbosity > 1)
	{
		if	(last)
			n += io_nputc(';', io, 1);

		if	(desc)
		{
			n += io_puts(" /* ", io);
			n += io_puts(desc, io);
			n += io_puts(" */", io);
		}
		
		n += io_puts("\n", io);
		n += io_nputc('\t', io, PrintTypeDepth);
		n += io_puts("}", io);
	}
	else	n += io_puts(" }", io);

	return n;
}


int PrintType (IO *io, const EfiType *type, int verbosity)
{
	int n = 0;

	if	(verbosity <= 0)
		return io_puts(type ? type->name : ".", io);

	if	(verbosity > 1)
		n += io_nputc('\t', io, PrintTypeDepth);

	if	(type == NULL)
		return n + io_puts(".", io);

	if	(type->flags & TYPE_EXTERN)
	{
		n += io_puts("extern ", io);

		if	(type->name && verbosity <= 2)
		{
			n += io_puts(type->name, io);
			n += show_version(io, type, 0);
			return n;
		}
		else if	(verbosity)
		{
			verbosity--;
		}
	}

	if	(type->list)
	{
		n += show_struct(io, type, verbosity);
	}
	else if	(type->dim)
	{
		PrintTypeDepth--;
		n += PrintType(io, type->base, verbosity);
		PrintTypeDepth++;
		n += io_printf(io, "[%d]", type->dim);
	}
/*
	else if	((type->flags & TYPE_ENUM) && type != &Type_enum)
*/
	else if	(type->flags & TYPE_ENUM)
	{
		n += show_enum(io, type, verbosity);
	}
	else if	(IsTypeClass(type, &Type_vec) && Val_ptr(type->defval))
	{
		EfiVec *vec = Val_ptr(type->defval);
		PrintTypeDepth--;
		n += PrintType(io, vec->type, verbosity);
		PrintTypeDepth++;
		n += io_puts("[]", io);
	}
	else
	{
		n += io_puts(type->name, io);
	}

	return n;
}

int ShowType (IO *io, const EfiType *type)
{
	return PrintType(io, type, 1);
}

char *Type2str (const EfiType *type)
{
	StrBuf *sb;
	IO *io;

	sb = sb_create(0);
	io = io_strbuf(sb);

	if	(type && type->name && type->name[0] != '_')
		PrintType(io, type, 0);
	else	PrintType(io, type, 1);

	io_close(io);
	return sb2str(sb);
}
