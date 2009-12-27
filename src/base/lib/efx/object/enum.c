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

			return msprintf("%#s", p->name);
		}
	}

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

void Enum_iconv (EfiFunc *func, void *rval, void **arg)
{
	Val_int(rval) = Val_int(arg[0]);
}

void Enum_sconv (EfiFunc *func, void *rval, void **arg)
{
	Val_int(rval) = EnumKeyCode(func->type, Val_str(arg[0]));
}

void Enum_to_str (EfiFunc *func, void *rval, void **arg)
{
	int val = Val_int(arg[0]);
	Val_str(rval) = EnumKeyLabel(func->arg[0].type, val, 0);
}

void Enum_fprint (EfiFunc *func, void *rval, void **arg)
{
	char *p = EnumKeyLabel(func->arg[1].type, Val_int(arg[1]), 1);
	Val_int(rval) = io_puts(p, Val_ptr(arg[0]));
	memfree(p);
}

static struct {
	char *fmt;
	void (*func) (EfiFunc *func, void *rval, void **arg);
} fdef[] = {
	{ "$1 $1 (int)", Enum_iconv },
	{ "int $1 ()", Enum_iconv },
	{ "$1 $1 (unsigned)", Enum_iconv },
	{ "unsigned $1 ()", Enum_iconv },
	{ "$1 $1 (str)", Enum_sconv },
	{ "restricted str $1 ()", Enum_to_str },
	{ "virtual int fprint (IO, $1)", Enum_fprint },
};

size_t Enum_read (const EfiType *type, void *data, IO *io)
{
	Val_int(data) = io_llget(io, type->recl);
	return type->recl;
}

size_t Enum_write (const EfiType *type, const void *data, IO *io)
{
	io_llput(Val_int(data), io, type->recl);
	return type->recl;
}

size_t Enum_uread (const EfiType *type, void *data, IO *io)
{
	Val_uint(data) = io_ullget(io, type->recl);
	return type->recl;
}

size_t Enum_uwrite (const EfiType *type, const void *data, IO *io)
{
	io_ullput(Val_uint(data), io, type->recl);
	return type->recl;
}

void Enum_copy (const EfiType *type, void *tg, const void *src)
{
	Val_int(tg) = Val_int(src);
}

size_t EnumTypeRecl (size_t dim)
{
	if	(dim < 0x7f)	return 1;
	if	(dim < 0x7fff)	return 2;

	return 4;
}

EfiType *NewEnumType (const char *name, size_t recl)
{
	EfiType *type;
	type = NewType(mstrcpy(name));
	type->cname = "int";
	type->size = sizeof(int);
	type->recl = recl;
	type->base = &Type_enum;
	type->read = Enum_read;
	type->write = Enum_write;
	type->copy = Enum_copy;
	type->vtab = VarTab(mstrcpy(type->name), 0);
	type->flags = TYPE_ENUM;
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

		sb = sb_acquire();
		sb_puts("_enum", sb);

		p = type->vtab->tab.data;
		k = type->vtab->tab.used;

		for (; k-- > 0; p++)
		{
			putname(p->name, sb);
			sb_printf(sb, "_%d", Val_int(p->obj->data));
		}

		sb_putc('_', sb);
		type->name = sb_cpyrelease(sb);
	}
	else	type->name = msprintf("_enum_%p", type);

	AddType(type);
	AddEnumFunc(type);
	return type;
}

void AddEnumFunc (EfiType *type)
{
	char *arg[2];
	int n;

	arg[0] = NULL;
	arg[1] = type->name;

	if	(!type->read)
		type->read = Enum_read;

	if	(!type->write)
		type->write = Enum_write;

	if	(!type->copy)
		type->copy = Enum_copy;

	type->flags |= TYPE_ENUM;

	for (n = tabsize(fdef); n-- > 0;)
	{
		char *p = mpsubvec(fdef[n].fmt, 2, arg);
		SetFunc(0, NULL, p, fdef[n].func);
		memfree(p);
	}
}

EfiType *MakeEnumType (const char *name, EnumTypeDef *def, size_t dim)
{
	EfiType *type = NewEnumType(name, EnumTypeRecl(dim));

	for (; dim-- > 0; def++)
		AddEnumKey(type, mstrcpy(def->name), mstrcpy(def->desc),
			def->val);

	return AddEnumType(type);
}
