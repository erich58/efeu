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

XTAB(TypeTab, 0, skey_cmp);


/*	Neuen Type generieren
*/

Type_t *NewType(char *name)
{
	Type_t *type;

	type = ALLOC(1, Type_t);
	memset(type, 0, sizeof(Type_t));
	/*
	type->name = name ? name : msprintf("T%#lx", (ulong_t) type);
	*/
	type->name = name;
	vb_init(&type->konv, 8, sizeof(Func_t *));
	return type;
}


void AddType (Type_t *type)
{
	Type_t *t2;

	if	(type->name == NULL)	return;

	t2 = xsearch(&TypeTab, type, XS_REPLACE);

	if	(t2 != NULL)
	{
		reg_str(1, t2->name);
		errmsg(MSG_EFMAIN, 157);
	}

	if	(type->vtab == NULL)
		type->vtab = VarTab(mstrcpy(type->name), 0);

	if	(type->defval == NULL)
	{
	/*
		Var_t *var;

		if	(type->dim)
			var = NewVar(type->base, mstrcpy("this"), type->dim);
		else	var = NewVar(type, mstrcpy("this"), 0);

		xsearch(&type->tab, var, XS_REPLACE);
	*/
		type->defval = memalloc(type->size);
		memset(type->defval, 0, type->size);
	}
}


Type_t *GetType (const char *name)
{
	return skey_find(&TypeTab, name);
}


void DelType (Type_t *type)
{
	DelVarTab(type->vtab);
	memfree(type->name);
}


static int cmp_struct (Var_t *a, Var_t *b);

Type_t *FindStruct(Var_t *list, size_t size)
{
	Type_t *type;
	int i;

	for (i = 0; i < TypeTab.dim; i++)
	{
		type = TypeTab.tab[i];

		if	(type && type->size == size &&
			 cmp_struct(list, type->list))
		{
			return type;
		}
	}

	return NULL;
}


static int cmp_struct(Var_t *a, Var_t *b)
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
