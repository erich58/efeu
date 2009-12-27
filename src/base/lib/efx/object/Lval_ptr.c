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

#include <object.h>

static Obj_t *ptr_alloc (Type_t *type, va_list list)
{
	Obj_t *obj = Obj_alloc(sizeof(Obj_t));
	obj->data = va_arg(list, void *);
	return obj;
}

static void ptr_free (Obj_t *obj)
{
	Obj_free(obj, sizeof(Obj_t));
}

static void ptr_update (Obj_t *obj)
{
	;
}

static void ptr_sync (Obj_t *obj)
{
	;
}

static char *ptr_ident (Obj_t *obj)
{
	return NULL;
}

Lval_t Lval_ptr = {
	ptr_alloc,
	ptr_free,
	ptr_update,
	ptr_sync,
	ptr_ident,
};
