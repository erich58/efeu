/*
Daten ausgeben

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

#include <EFEU/printobj.h>

char *PrintListBegin = "{";
char *PrintListDelim = ", ";
char *PrintListEnd = "}";

Func_t *GetPrintFunc(const Type_t *type)
{
	return GetFunc(&Type_int, GetGlobalFunc("fprint"),
		2, &Type_io, 0, type, 0);
}

static int print_struct (io_t *io, const Var_t *list, const void *data)
{
	char *delim;
	const void *ptr;
	int n;

	n = io_puts(PrintListBegin, io);
	delim = NULL;

	for (; list != NULL; list = list->next)
	{
		n += io_puts(delim, io);
		delim = PrintListDelim;
		ptr = (const char *) data + list->offset;

		if	(list->dim)
		{
			n += PrintVecData(io, list->type, ptr, list->dim);
		}
		else	n += PrintData(io, list->type, ptr);
	}

	n += io_puts(PrintListEnd, io);
	return n;
}

int PrintVecData(io_t *io, const Type_t *type, const void *data, size_t dim)
{
	Func_t *func;
	char *delim;
	int i, n;
	int buf;

	func = GetPrintFunc(type);
	n = io_puts(PrintListBegin, io);
	delim = NULL;

	for (i = 0; i < dim; i++)
	{
		n += io_puts(delim, io);
		delim = PrintListDelim;

		if	(func)
		{
			buf = 0;
			CallFunc(&Type_int, &buf, func, &io, data);
			n += buf;
		}
		else	n += PrintData(io, type, data);

		data = (const char *) data + type->size;
	}

	n += io_puts(PrintListEnd, io);
	return n;
}


int PrintData(io_t *io, const Type_t *type, const void *data)
{
	Func_t *func;

	FuncDebugLock++;
	func = GetPrintFunc(type);
	FuncDebugLock--;

	if	(func != NULL)
	{
		int n = 0;
		CallFunc(&Type_int, &n, func, &io, data);
		return n;
	}
	else	return PrintAny(io, type, data);
}


int PrintObj(io_t *io, const Obj_t *obj)
{
	return (obj ? PrintData(io, obj->type, obj->data) : io_puts("void", io));
}


int PrintAny (io_t *io, const Type_t *type, const void *data)
{
	if	(type->dim)
	{
		return PrintVecData(io, type->base, data, type->dim);
	}
	else if	(type->list)
	{
		return print_struct(io, type->list, data);
	}
	else if	(type->base)
	{
		return PrintData(io, type->base, data);
	}
	else
	{
		int i, n;

		n = io_printf(io, "(%s) 0x", type->name);

		for (i = 0; i < type->size; i++)
			n += io_printf(io, "%02x", ((uchar_t *) data)[i]);

		return n;
	}
}
