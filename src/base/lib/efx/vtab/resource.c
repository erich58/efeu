/*
Resourceabfrage

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
#include <EFEU/Resource.h>

static EfiObj *res_alloc (EfiType *type, va_list list)
{
	EfiObj *obj = Obj_alloc(sizeof(EfiObj) + sizeof(char **) + type->size);
	char **name = (void *) (obj + 1);
	obj->data = (void *) (name + 1);
	*name = mstrcpy(va_arg(list, char *));
	return obj;
}

static void res_free (EfiObj *obj)
{
	char **name = (void *) (obj + 1);
	memfree(*name);
	CleanData(obj->type, obj->data, 1);
}

static void res_update (EfiObj *obj)
{
	char *p = mstrcpy(GetResource(Val_str(obj + 1), NULL));
	Obj2Data(str2Obj(p), obj->type, obj->data);
}

static void res_sync (EfiObj *obj)
{
	char *p = Obj2str(ConstObj(obj->type, obj->data));
	SetResource(Val_str(obj + 1), p);
}

static char *res_ident (const EfiObj *obj)
{
	char **name = (void *) (obj + 1);
	return msprintf("Resource: %s", *name);
}

static EfiLval Lval_res = {
	"res", "Resource",
	res_alloc,
	res_free,
	res_update,
	res_sync,
	NULL,
	res_ident,
};


EfiObj *ResourceObj (EfiType *type, const char *name)
{
	return LvalObj(&Lval_res, type, name);
}
