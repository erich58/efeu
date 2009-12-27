/*
Aufzählungstypen verwalten

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
#include <EFEU/stdtype.h>

void AddEnumKey (EfiType *type, char *name, int val)
{
	VarTab_xadd(type->vtab, name, NULL, NewObj(type, &val));
}

char *EnumKeyLabel (const EfiType *type, int val, int flag)
{
	VarTabEntry *p;
	const EfiType *t;
	size_t n;

	for (t = type; t != NULL; t = t->base)
	{ 
		if	(t->vtab == NULL)	continue;

		p = t->vtab->tab.data;
		n = t->vtab->tab.used;

		for (; n-- > 0; p++)
		{
			if	(p->obj == NULL)
				continue;

			if	(p->obj->type != t)
				continue;

			if	(Val_int(p->obj->data) != val)
				continue;

			if	(flag)
				return mstrpaste("::", t->name, p->name);

			return mstrcpy(p->name);
		}
	}

	if	(flag)
		return msprintf("(%s) %d", type->name, val);

	return msprintf("%d", val);
}

static EfiObjList **add_key (EfiObjList **ptr, EfiType *type)
{
	if	(type == NULL)		return NULL;
	if	(type == &Type_enum)	return ptr;

	ptr = add_key(ptr, type->base);

	if	(ptr && type->vtab)
	{
		VarTabEntry *p;
		size_t n;

		p = type->vtab->tab.data;
		n = type->vtab->tab.used;

		for (; n-- > 0; p++)
		{
			if	(p->obj && p->obj->type == type)
			{
				*ptr = NewObjList(RefObj(p->obj));
				ptr = &(*ptr)->next;
			}
		}
	}

	return ptr;
}

EfiObjList *EnumKeyList (EfiType *type)
{
	EfiObjList *list = NULL;
	add_key(&list, type);
	return list;
}

int EnumKeyCode (const EfiType *type, const char *name)
{
	VarTabEntry *var;

	if	(!name || !type->vtab)	return 0;

	while (type != NULL)
	{
		var = VarTab_get(type->vtab, name);

		if	(var && var->obj && var->obj->type == type)
			return Val_int(var->obj->data);

		type = type->base;
	}

	return 0;
}

static void ikonv (EfiFunc *func, void *rval, void **arg)
{
	Val_int(rval) = Val_int(arg[0]);
}

static void str2enum (EfiFunc *func, void *rval, void **arg)
{
	Val_int(rval) = EnumKeyCode(func->type, Val_str(arg[0]));
}

static void enum2str (EfiFunc *func, void *rval, void **arg)
{
	int val = Val_int(arg[0]);
	Val_str(rval) = EnumKeyLabel(func->arg[0].type, val, 0);
}

static struct {
	char *fmt;
	void (*func) (EfiFunc *func, void *rval, void **arg);
} fdef[] = {
	{ "$1 int ()", ikonv },
	{ "restricted int $1 ()", ikonv },
	{ "$1 str ()", str2enum },
	{ "restricted str $1 ()", enum2str },
};


EfiType *NewEnumType (const char *name, EnumTypeDef *def, size_t dim)
{
	char *arg[2];
	EfiType *type;
	int n;
	
	type = NewType(mstrcpy(name));
	type->size = sizeof(int);
	type->recl = sizeof(int);
	type->base = &Type_enum;
	AddType(type);

	for (; dim-- > 0; def++)
		AddEnumKey(type, mstrcpy(def->name), def->val);

	arg[0] = NULL;
	arg[1] = mstrcpy(type->name);

	for (n = tabsize(fdef); n-- > 0;)
	{
		char *p = mpsubvec(fdef[n].fmt, 2, arg);
		SetFunc(0, NULL, p, fdef[n].func);
		memfree(p);
	}

	memfree(arg[1]);
	return type;
}
