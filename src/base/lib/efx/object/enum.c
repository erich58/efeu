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

static Obj_t *enum_member (const Var_t *member, const Obj_t *obj)
{
	int val = (int) member->par;
	return NewObj(member->type, &val);
}

void AddEnumKey (Type_t *type, const char *name, int val)
{
	MemberDef_t member;
	member.name = (char *) name;
	member.type = type;
	member.member = enum_member;
	member.par = (void *) val;
	member.desc = NULL;
	AddMember(type->vtab, &member, 1);
}

char *EnumKeyLabel (const Type_t *type, int val, int flag)
{
	xtab_t *tab;
	Var_t *var;
	const Type_t *t;
	int i;

	for (t = type; t != NULL; t = t->base)
	{ 
		if	(t->vtab == NULL)	continue;

		tab = &t->vtab->tab;

		for (i = 0; i < tab->dim; i++)
		{
			var = tab->tab[i];

			if	(var->type != t)
				continue;

			if	((int) var->par != val)
				continue;

			if	(flag)
				return mstrpaste("::", t->name, var->name);

			return mstrcpy(var->name);
		}
	}

	if	(flag)
		return msprintf("(%s) %d", type->name, val);

	return msprintf("%d", val);
}

static ObjList_t **add_key (ObjList_t **ptr, Type_t *type)
{
	if	(type == NULL)		return NULL;
	if	(type == &Type_enum)	return ptr;

	ptr = add_key(ptr, type->base);

	if	(ptr)
	{
		xtab_t *tab;
		Var_t *var;
		int i;

		if	(type->vtab == NULL)	return ptr;

		tab = &type->vtab->tab;

		for (i = 0; i < tab->dim; i++)
		{
			var = tab->tab[i];

			if	(var->type != type)
				continue;

			*ptr = NewObjList(enum_member(var, NULL));
			ptr = &(*ptr)->next;
		}
	}

	return ptr;
}

ObjList_t *EnumKeyList (Type_t *type)
{
	ObjList_t *list = NULL;
	add_key(&list, type);
	return list;
}

int EnumKeyCode (const Type_t *type, const char *name)
{
	Var_t key, *var;

	if	(!name || !type->vtab)	return 0;

	key.name = (char *) name;

	while (type != NULL)
	{
		var = xsearch(&type->vtab->tab, &key, XS_FIND);

		if	(var && var->type == type)
			return (int) var->par;

		type = type->base;
	}

	return 0;
}

static void ikonv (Func_t *func, void *rval, void **arg)
{
	Val_int(rval) = Val_int(arg[0]);
}

static void str2enum (Func_t *func, void *rval, void **arg)
{
	Val_int(rval) = EnumKeyCode(func->type, Val_str(arg[0]));
}

static void enum2str (Func_t *func, void *rval, void **arg)
{
	int val = Val_int(arg[0]);
	Val_str(rval) = EnumKeyLabel(func->arg[0].type, val, 0);
}

static struct {
	char *fmt;
	FuncEval_t func;
} fdef[] = {
	{ "$1 int ()", ikonv },
	{ "restricted int $1 ()", ikonv },
	{ "$1 str ()", str2enum },
	{ "restricted str $1 ()", enum2str },
};


Type_t *NewEnumType (const char *name, EnumTypeDef_t *def, size_t dim)
{
	Type_t *type;
	int n;
	
	type = NewType(mstrcpy(name));
	type->size = sizeof(int);
	type->recl = sizeof(int);
	type->iodata = IOData_std;
	type->base = &Type_enum;
	AddType(type);

	for (; dim-- > 0; def++)
		AddEnumKey(type, def->name, def->val);

	reg_set(1, mstrcpy(type->name));

	for (n = tabsize(fdef); n-- > 0;)
	{
		char *p = parsub(fdef[n].fmt);
		SetFunc(0, NULL, p, fdef[n].func);
		memfree(p);
	}

	return type;
}
