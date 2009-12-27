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
#include <ctype.h>

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

static int show_enum (IO *io, const EfiType *type, int verbosity)
{
	int n;
	char *delim;

	n = io_puts("enum", io);

	if	(type->name && type->name[0] != '_')
	{
		n += io_nputc(' ', io, 1);
		n += io_puts(type->name, io);
	}

	delim = verbosity > 1 ? "\n\t" : " ";

	if	(type->base != &Type_enum)
	{
		n += io_puts(" : ", io);
		n += show_enum(io, type->base, verbosity);
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
				n += io_puts(delim, io);

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
					n += io_printf(io, "\t/* %s */",
						p->desc);
			}
		}
	}

	n += io_puts(verbosity > 1 ? "\n}" : " }", io);
	return n;
}

static int show_struct (IO *io, const EfiType *type, int verbosity)
{
	EfiVar *st;
	EfiType *last;
	char *delim;
	char *desc;
	int n;

	n = io_puts("struct", io);

	if	(type->name && type->name[0] != '_')
	{
		n += io_nputc(' ', io, 1);
		n += io_puts(type->name, io);
	}

	if	(type->base)
	{
		n += io_puts(" : ", io);
		n += PrintType(io, type->base, verbosity);
	}

	n += io_puts(" {", io);
	delim = verbosity > 1 ? "\n\t" : " ";
	n += io_puts(delim, io);
	last = NULL;
	desc = NULL;

	for (st = type->list; st != NULL; st = st->next)
	{
		if	(!st->name)	continue;

		if	(verbosity > 1 || st->type != last)
		{
			if	(last != NULL)
			{
				n += io_puts(";", io);

				if	(desc)
				{
					n += io_puts(" /* ", io);
					n += io_puts(desc, io);
					n += io_puts(" */", io);
				}

				n += io_puts(delim, io);
			}

			if	(st->type == type->base)
				n += io_puts(st->type->name, io);
			else	n += ShowType(io, st->type);
		}
		else	n += io_nputc(',', io, 1);

		n += io_nputc(' ', io, 1);
		n += io_puts(st->name, io);

		if	(st->dim > 1)
			n += io_printf(io, "[%d]", st->dim);

		last = st->type;

		if	(verbosity > 1)
			desc = st->desc;
	}

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
		
		n += io_puts("\n}", io);
	}
	else	n += io_puts(" }", io);

	return n;
}


int PrintType (IO *io, const EfiType *type, int verbosity)
{
	if	(type == NULL)
		return io_puts(".", io);

	if	(verbosity <= 0)
		return io_puts(type->name, io);

	if	(IsTypeClass(type, &Type_enum) && type != &Type_enum)
		return show_enum(io, type, verbosity);

	if	(type->list)
		return show_struct(io, type, verbosity);

	if	(type->dim)
	{
		int n = PrintType(io, type->base, 1);
		n += io_printf(io, "[%d]", type->dim);
		return n;
	}

	if	(type->base == &Type_vec)
	{
		EfiVec *vec = Val_ptr(type->defval);
		int n = PrintType(io, vec->type, 1);
		n += io_puts("[]", io);
		return n;
	}

	return io_puts(type->name, io);
}

int ShowType (IO *io, const EfiType *type)
{
	return PrintType(io, type, 1);
}
