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
#include <ctype.h>

static void DelType (void *ptr)
{
	EfiType *type = ptr;
	DelVarTab(type->vtab);
	memfree(type->name);
}

NameKeyTab TypeTab = NKT_DATA("Type", 60, DelType);


/*	Neuen Type generieren
*/

EfiType *NewType (char *name)
{
	EfiType *type;

	type = memalloc(sizeof(EfiType));
	/*
	type->name = name ? name : msprintf("T%#p", type);
	*/
	type->name = name;
	vb_init(&type->konv, 8, sizeof(EfiFunc *));
	return type;
}


void AddType (EfiType *type)
{
	if	(type->name == NULL)	return;

	if	(type->vtab == NULL)
		type->vtab = VarTab(mstrcpy(type->name), 0);

	if	(type->defval == NULL)
	{
		type->defval = memalloc(type->size);
		memset(type->defval, 0, type->size);
	}

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


static int cmp_struct (EfiVar *a, EfiVar *b)
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

EfiType *FindStruct (EfiVar *list, size_t size)
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
