/*
Daten ausgeben

$Copyright (C) 1994, 2006 Erich Frühstück
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

#include <EFEU/printobj.h>

int PrintObj (IO *io, const EfiObj *obj)
{
	return (obj ? PrintData(io, obj->type, obj->data) :
		io_puts("void", io));
}

static int print_struct (IO *io, EfiType *type, const void *data)
{
	char *delim;
	EfiStruct *list;
	EfiObj *obj, *base;
	int n;

	list = type->list;
	n = io_puts("{", io);
	delim = NULL;
	base = LvalObj(&Lval_ptr, type, data);

	for (; list != NULL; list = list->next)
	{
		n += io_puts(delim, io);
		delim = ", ";
		obj = Var2Obj(list, base);
		n += PrintObj(io, obj);
		UnrefObj(obj);
	}

	n += io_puts("}", io);
	UnrefObj(base);
	return n;
}

int PrintData (IO *io, const EfiType *type, const void *data)
{
	if	(type->print)
	{
		return type->print(type, data, io);
	}
	else if	(type->dim)
	{
		return PrintVecData(io, type->base, data, type->dim);
	}
	else if	(type->list)
	{
		return print_struct(io, (EfiType *) type, data);
	}
	else if	(IsTypeClass(type, &Type_enum))
	{
		return io_xprintf(io, "%d", Val_int(data));
	}
	else if	(type->base)
	{
		int n = io_xprintf(io, "(%s) ", type->name);
		return n + PrintData(io, type->base, data);
	}
	else if	(type->size)
	{
		int i, n;
		const unsigned char *p;

		n = io_xprintf(io, "(%s) 0x", type->name);
		p = data;

		for (i = 0; i < type->size; i++)
			n += io_xprintf(io, "%02x", p[i]);

		return n;
	}
	else	return io_xprintf(io, "%s()", type->name);
}

int PrintVecData (IO *io, const EfiType *type, const void *data, size_t dim)
{
	char *delim;
	int i, n;

	n = io_puts("{", io);
	delim = NULL;

	for (i = 0; i < dim; i++)
	{
		n += io_puts(delim, io);
		n += PrintData(io, type, data);
		data = (const char *) data + type->size;
		delim = ", ";
	}

	n += io_puts("}", io);
	return n;
}
