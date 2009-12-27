/*
Datentypen verwalten

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
#include <EFEU/EfiStat.h>
#include <EFEU/EfiPar.h>
#include <ctype.h>

void DelType (EfiType *type)
{
	DelVarTab(type->vtab);
	memfree(type->name);
}

static void del_type (void *ptr)
{
	DelType(ptr);
}

NameKeyTab TypeTab = NKT_DATA("Type", 60, del_type);
unsigned TypeTabChangeCount = 0;


/*	Neuen Type generieren
*/

EfiType *NewType (char *name)
{
	EfiType *type;

	type = memalloc(sizeof(EfiType));
	memset(type, 0, sizeof *type);
	type->name = name;
	vb_init(&type->konv, 8, sizeof(EfiFunc *));
	vb_init(&type->par, 16, sizeof(EfiParClass *));
	return type;
}

void AddType (EfiType *type)
{
	EfiStat *efi = Efi_ptr(NULL);

	if	(type == NULL)		return;
	if	(type->name == NULL)	return;
	if	(type->order)		return;

	AddType(type->base);

	if	(type->vtab == NULL)
		type->vtab = VarTab(mstrcpy(type->name), 0);

	if	(type->defval == NULL)
	{
		type->defval = memalloc(type->size);
		memset(type->defval, 0, type->size);
	}

	if	(!type->src)
		type->src = rd_refer(efi->src);

	TypeTabChangeCount++;

	if	(!type->order)
		type->order = TypeTabChangeCount;

	if	(nkt_insert(&TypeTab, type->name, type))
		dbg_note(NULL, "[efmain:157]", "s", type->name);
}

EfiType *GetType (const char *name)
{
	return nkt_fetch(&TypeTab, name, NULL);
}

int IsTypeClass (const EfiType *type, const EfiType *base)
{
	for (; type != NULL; type = type->base)
		if (type == base) return 1;

	return 0;
}

static int cmp_struct (EfiStruct *a, EfiStruct *b)
{
	while (a != NULL || b != NULL)
	{
		if	(a == NULL || b == NULL)		return 0;
		if	(a->type != b->type)			return 0;
		if	(a->dim != b->dim)			return 0;
		if	(a->offset != b->offset)		return 0;
		if	(mstrcmp(a->name, b->name) != 0)	return 0;

		a = a->next;
		b = b->next;
	}

	return 1;
}

EfiType *FindStruct (EfiStruct *list, size_t size)
{
	EfiType *type;
	NameKeyEntry *ptr;
	size_t n;

	for (ptr = TypeTab.tab.data, n = TypeTab.tab.used; n-- > 0; ptr++)
	{
		type = ptr->data;

		if	(type && type->size == size &&
			 cmp_struct(list, type->list))
		{
			return type;
		}
	}

	return NULL;
}

static int cmp_entry (VarTabEntry *a, VarTabEntry *b, size_t dim)
{
	for (; dim-- > 0; a++, b++)
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

static int test_type (EfiType *type, VarTabEntry *tab, size_t dim)
{
	if	(!IsTypeClass(type, &Type_enum))
		return 0;

	if	(!type->vtab)
		return 0;
	
	if	(type->vtab->tab.used != dim)
		return 0;

	if	(cmp_entry(type->vtab->tab.data, tab, dim))
		return 1;

	return 0;
}

EfiType *FindEnum (EfiType *base)
{
	VarTabEntry *entry;
	size_t dim;
	EfiType *type;
	NameKeyEntry *ptr;
	size_t n;

	if	(!base->vtab)
		return NULL;

	entry = base->vtab->tab.data;
	dim = base->vtab->tab.used;

	for (n = 0; n < dim; n++)
		if (!IsTypeClass(entry[n].type, &Type_enum))
			return NULL;

	if	(base->name && (type = GetType(base->name)))
	{
		if	(test_type(type, entry, dim))
			return type;

		return NULL;
	}

	for (ptr = TypeTab.tab.data, n = TypeTab.tab.used; n-- > 0; ptr++)
		if (test_type(ptr->data, entry, dim))
			return ptr->data;

	return NULL;
}
