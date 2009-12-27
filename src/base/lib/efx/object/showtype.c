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

int ShowType(IO *io, const EfiType *type)
{
	EfiVar *st;
	EfiType *last;
	int n;

	if	(type == NULL)
		return io_puts(".", io);

	n = 0;

	if	(type->list)
		n += io_puts("struct ", io);

	if	(type->name)
		n += io_puts(type->name, io);
	
	if	(type->list)
	{
		n += io_puts(" { ", io);
		last = NULL;

		for (st = type->list; st != NULL; st = st->next)
		{
			if	(st->type != last)
			{
				if	(last != NULL)
					n += io_puts("; ", io);

				n += ShowType(io, st->type);
			}
			else	n += io_nputc(',', io, 1);

			n += io_nputc(' ', io, 1);
			n += io_puts(st->name, io);

			if	(st->dim > 1)
				n += io_printf(io, "[%d]", st->dim);

			last = st->type;
		}

		n += io_puts(" }", io);
	}

	return n;
}


static void list_name(IO *io, EfiVar *st)
{
	ShowType(io, st->type);

	if	(st->name == NULL)
		io_puts(" <noname>", io);
	else if	(isalpha(st->name[0]) || st->name[0] == '_')
		io_printf(io, " %s", st->name);
	else	io_printf(io, " operator%s", st->name);

	if	(st->dim > 1)	io_printf(io, "[%d]", st->dim);
}

static void list_func(IO *io, const char *pfx, EfiFunc *func)
{
	io_puts(pfx, io);
	ListFunc(io, func);
	io_putc('\n', io);
}

static void list_vfunc(IO *io, const char *pfx, const VecBuf *tab)
{
	EfiFunc **ftab;
	int i;

	io_puts(pfx, io);

	if	(tab == NULL)
	{
		io_puts("NULL\n", io);
		return;
	}

	ftab = (EfiFunc **) tab->data;

	if	(pfx == NULL)
	{
		for (i = 0; i < tab->used; i++)
			list_func(io, NULL, ftab[i]);
	}
	else if	(tab->used > 1)
	{
		io_puts("{\n", io);

		for (i = 0; i < tab->used; i++)
			list_func(io, "\t", ftab[i]);

		io_puts("}\n", io);
	}
	else
	{
		io_puts("{ ", io);

		if	(tab->used)
			ListFunc(io, ftab[0]);

		io_puts(" }\n", io);
	}
}

static void show_base (IO *io, const EfiType *type)
{
	ShowType(io, type);

	if	(type->base)
	{
		io_putc('(', io);
		show_base(io, type->base);
		io_putc(')', io);
	}
}

void ListType(IO *io, const EfiType *type)
{
	EfiVar *st;
	size_t size, tsize;

	if	(type == NULL)	return;

	io_puts("name = ", io);
	ShowType(io, type);
	io_printf(io, ", size = %d", type->size);
	io_printf(io, ", recl = %d", type->recl);

	if	(type->base)
	{
		io_puts(", base = ", io);
		show_base(io, type->base);
	}

	if	(type->defval)
	{
		io_puts(", defval = ", io);
		PrintData(io, type, type->defval);
	}

	if	(type->eval)	io_puts(", eval()", io);
	if	(type->read)	io_puts(", read()", io);
	if	(type->write)	io_puts(", write()", io);
	if	(type->copy)	io_puts(", copy()", io);
	if	(type->clean)	io_puts(", clean()", io);

	io_putc('\n', io);
	tsize = 0;

	for (st = type->list; st != NULL; st = st->next)
	{
		if	(tsize != st->offset)
			io_printf(io, "@%d:%d\n", tsize, st->offset - tsize);

		size = st->type->size * (st->dim ? st->dim : 1);
		tsize = st->offset + size;
		io_printf(io, "@%d:%d\t", st->offset, size);
		list_name(io, st);
		io_putc('\n', io);
	}

	if	(type->fcopy)	list_func(io, "copy = ", type->fcopy);
	if	(type->fclean)	list_func(io, "clean = ", type->fclean);
	if	(type->create)	list_vfunc(io, "create = ", &type->create->tab);

	list_vfunc(io, "konv = ", &type->konv);
	ShowVarTab(io, "var", type->vtab);
	io_putc('\n', io);
}
