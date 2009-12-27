/*
Funktion auflisten

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
#include <EFEU/ioctrl.h>
#include <ctype.h>

#define	BASE_FONT	IO_ROMAN_FONT
#define	NAME_FONT	IO_ITALIC_FONT


static int show_farg (io_t *io, FuncArg_t *arg);

static int lf_type (io_t *io, const Type_t *type, const char *repl)
{
	return io_puts(type ? type->name : repl, io);
}

static int lf_name (io_t *io, const char *name)
{
	if	(name == NULL)
		return 0;
	if	(!isalpha(name[0]))
		return io_printf(io, "operator%#s", name);
	return io_puts(name, io);
}


int ListFunc(io_t *io, Func_t *func)
{
	int i, n;
	char *p;

	if	(func == NULL)
		return 0;

	n = 0;

	if	(func->virtual)
		n += io_puts("virtual ", io);

	switch (func->weight)
	{
	case KONV_PROMOTION:	n += io_puts("promotion ", io); break;
	case KONV_RESTRICTED:	n += io_puts("restricted ", io); break;
	default:		break;
	}

	if	(func->eval == Func_inline)
		n += io_puts("inline ", io);

	n += lf_type(io, func->type, Type_obj.name);
	n += io_puts(func->lretval ? " & " : " ", io);

	if	(func->bound)
	{
		if	(func->name == NULL && func->type == NULL
				&& func->arg[0].lval)
			n += io_puts("~", io);

		n += lf_type(io, func->arg[0].type, Type_obj.name);

		if	(func->name)
		{
			n += io_puts("::", io);
			n += lf_name(io, func->name);
		}

		if	(func->arg[0].lval)
			io_puts(" &", io);
	}
	else if	(func->name == NULL)
	{
		n += lf_type(io, func->type, Type_obj.name);
	}
	else	n += lf_name(io, func->name);

	n += io_puts(" (", io);
	p = NULL;
	i = func->bound ? 1 : 0;

	while (i < func->dim)
	{
		n += io_puts(p, io);
		n += show_farg(io, func->arg + i);
		p = ", ";
		i++;
	}

	n += io_puts(")", io);
	return n;
}



static int show_farg(io_t *io, FuncArg_t *arg)
{
	int n;

	n = 0;

	if	(arg->cnst)	n += io_puts("const ", io);

	n += lf_type(io, arg->type, ".");

	if	(arg->lval)	n += io_puts(" &", io);
	if	(arg->nokonv)	n += io_puts(" *", io);

	if	(arg->name)
	{
		n += io_puts(" ", io);
		io_ctrl(io, NAME_FONT);
		n += io_puts(arg->name, io);
		io_ctrl(io, BASE_FONT);
	}

	if	(arg->defval)
	{
		n += io_puts(" = ", io);
		n += PrintObj(io, arg->defval);
	}

	return n;
}
