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
#include <ctype.h>

void AddEnumKey (EfiType *type, char *name, char *desc, int val)
{
	VarTab_xadd(type->vtab, name, desc, NewObj(type, &val));
}

static int name_is_std (const char *str)
{
	if	(!str)	return 0;
	if	(*str != '_' && !isalpha(*str))	return 0;

	while (*(++str) != 0)
		if (*str != '_' && !isalnum(*str)) return 0;

	return 1;
}

char *EnumKeyLabel (const EfiType *type, int val, int flag)
{
	VarTabEntry *p;
	const EfiType *t;
	size_t n;
	int mode;

	for (t = type; t != NULL; t = t->base)
	{ 
		if	(t->vtab == NULL)	continue;

		mode = (t->name && t->name[0] != '_');
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

			if	(!flag)
				return mstrcpy(p->name);

			if	(!mode)
				return msprintf("%#s", p->name);

			if	(name_is_std(p->name))
				return mstrpaste("::", t->name, p->name);

			return msprintf("%s::operator%#s", t->name, p->name);
		}
	}

	if	(flag && type->name && type->name[0] != '_')
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

static void fprint_enum (EfiFunc *func, void *rval, void **arg)
{
	char *p = EnumKeyLabel(func->arg[1].type, Val_int(arg[1]), 1);
	Val_int(rval) = io_puts(p, Val_ptr(arg[0]));
	memfree(p);
}

static struct {
	char *fmt;
	void (*func) (EfiFunc *func, void *rval, void **arg);
} fdef[] = {
	{ "$1 $1 (int)", ikonv },
	{ "int $1 ()", ikonv },
	{ "unsigned $1 ()", ikonv },
	{ "$1 $1 (str)", str2enum },
	{ "restricted str $1 ()", enum2str },
	{ "virtual int fprint (IO, $1)", fprint_enum },
};


EfiType *NewEnumType (const char *name)
{
	EfiType *type;
	type = NewType(mstrcpy(name));
	type->cname = "int";
	type->size = sizeof(int);
	type->recl = sizeof(int);
	type->base = &Type_enum;
	type->vtab = VarTab(mstrcpy(type->name), 0);
	return type;
}

static void putname (const char *name, StrBuf *sb)
{
	if	(!name)	return;

	sb_putc('_', sb);

	while (*name)
	{
		sb_putc(isalnum((unsigned char) *name) ? *name : '_', sb);
		name++;
	}
}

static int test_enum (EfiType *base, EfiType *type)
{
	VarTabEntry *a, *b;
	size_t n;

	if	(base->base != type->base)	return 0;

	if	(!base->vtab)		return 0;
	if	(!type->vtab)		return 0;

	if	(base->vtab->tab.used != type->vtab->tab.used)
			return 0;

	a = base->vtab->tab.data;
	b = type->vtab->tab.data;

	for (n = type->vtab->tab.used; n-- > 0; a++, b++)
	{
		if	(mstrcmp(a->name, b->name) != 0)
			return 0;

		if	(!IsTypeClass(a->type, &Type_enum))
			return 0;

		if	(Val_int(a->obj->data) != Val_int(b->obj->data))
			return 0;
	}

	return 1;
}

EfiType *AddEnumType (EfiType *type)
{
	EfiType *base;
	char *arg[2];
	int n;

	if	(type->name)
	{
		base = GetType(type->name);

		if	(base && test_enum(base, type))
		{
			DelType(type);
			return base;
		}
	}
	else if	((base = FindEnum(type)))
	{
		DelType(type);
		return base;
	}
	else if	(type->vtab)
	{
		StrBuf *sb;
		VarTabEntry *p;
		size_t k;

		sb = sb_create(0);
		sb_puts("_enum", sb);

		p = type->vtab->tab.data;
		k = type->vtab->tab.used;

		for (; k-- > 0; p++)
		{
			putname(p->name, sb);
			sb_printf(sb, "_%d", Val_int(p->obj->data));
		}

		sb_putc('_', sb);
		type->name = sb2str(sb);
	}
	else	type->name = msprintf("_enum_%p", type);

	AddType(type);

	arg[0] = NULL;
	arg[1] = type->name;

	for (n = tabsize(fdef); n-- > 0;)
	{
		char *p = mpsubvec(fdef[n].fmt, 2, arg);
		SetFunc(0, NULL, p, fdef[n].func);
		memfree(p);
	}

	return type;
}

EfiType *EnumType (const char *name)
{
	EfiType *type = NewEnumType(name);
	return AddEnumType(type);
}

void MakeEnumType (const char *name, EnumTypeDef *def, size_t dim)
{
	EfiType *type = NewEnumType(name);

	for (; dim-- > 0; def++)
		AddEnumKey(type, mstrcpy(def->name), NULL, def->val);

	AddEnumType(type);
}
