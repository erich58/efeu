/*
Daten - L-Wert

$Copyright (C) 2001 Erich Frühstück
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

static EfiObj *data_alloc (EfiType *type, va_list list)
{
	EfiObj *obj = Obj_alloc(type->size + sizeof *obj);
	obj->data = (obj + 1);

	memset(obj->data, 0, type->size);

	if	(type->defval)
		CopyData(type, obj->data, type->defval);

	return obj;
}

static EfiObj *data_xalloc (EfiType *type, va_list list)
{
	EfiObj *obj = Obj_alloc(type->size + sizeof *obj);
	void *data = va_arg(list, void *);
	obj->data = (obj + 1);

	memset(obj->data, 0, type->size);

	if	(data)
		CopyData(type, obj->data, data);
	else if	(type->defval)
		CopyData(type, obj->data, type->defval);

	return obj;
}

static void data_free (EfiObj *obj)
{
	CleanData(obj->type, obj->data, 1);
	Obj_free(obj, obj->type->size + sizeof *obj);
}

static void data_update (EfiObj *obj)
{
	;
}

static void data_sync (EfiObj *obj)
{
	;
}

static char *data_ident (const EfiObj *obj)
{
	return NULL;
}

EfiLval Lval_data = {
	"data",
	NULL,
	data_alloc,
	data_free,
	data_update,
	data_sync,
	data_ident,
};

EfiLval Lval_xdata = {
	"xdata",
	NULL,
	data_xalloc,
	data_free,
	data_update,
	data_sync,
	data_ident,
};
