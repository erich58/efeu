/*
Pointer - L-Wert

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

static EfiObj *ptr_alloc (EfiType *type, va_list list)
{
	EfiObj *obj = Obj_alloc(sizeof(EfiObj));
	obj->data = va_arg(list, void *);
	return obj;
}

static void ptr_free (EfiObj *obj)
{
	Obj_free(obj, sizeof(EfiObj));
}

static void ptr_update (EfiObj *obj)
{
	;
}

static void ptr_sync (EfiObj *obj)
{
	;
}

static char *ptr_ident (const EfiObj *obj)
{
	return NULL;
}

EfiLval Lval_ptr = {
	ptr_alloc,
	ptr_free,
	ptr_update,
	ptr_sync,
	ptr_ident,
};
