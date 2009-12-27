/*
Variable eintragen

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

#define	KEY_THIS	"this"

static int var_cmp (const void *pa, const void *pb)
{
	const VarTabEntry *a = pa;
	const VarTabEntry *b = pb;
	return mstrcmp(a->name, b->name);
}

static void var_clean (VarTabEntry *var)
{
	if	(var)
	{
		memfree((char *) var->name);
		memfree(var->desc);
		UnrefObj(var->obj);

		if	(var->clean)
			var->clean(var->data);
	}
}

static VarTabEntry *var_get (EfiVarTab *tab, const char *name, int mode)
{
	if	(tab)
	{
		VarTabEntry key;
		key.name = name;
		return vb_search(&tab->tab, &key, var_cmp, mode);
	}

	return NULL;
}

VarTabEntry *VarTab_get (EfiVarTab *tab, const char *name)
{
	EfiVarStack *stack;
	VarTabEntry *ptr;

	if	(tab)
		return var_get(tab, name, VB_SEARCH);

	if	((ptr = var_get(LocalVar, name, VB_SEARCH)))
		return ptr;

	for (stack = VarStack; stack != NULL; stack = stack->next)
		if ((ptr = var_get(stack->tab, name, VB_SEARCH)))
			return ptr;

	return NULL;
}

static EfiObj *getvar (VarTabEntry *key, EfiVarTab *tab, const EfiObj *obj)
{
	if	(tab == NULL)	return NULL;

	if	(obj && mstrcmp(key->name, KEY_THIS) == 0)
		return RefObj(obj);

	key = vb_search(&tab->tab, key, var_cmp, VB_SEARCH);

	if	(!key)	return NULL;

	if	(key->get)
		return key->get(obj, key->data);

	return RefObj(key->obj);
}

EfiObj *GetVar (EfiVarTab *tab, const char *name, const EfiObj *obj)
{
	VarTabEntry key;
	EfiVarStack *stack;
	EfiObj *x;

	if	(name == NULL)	return NULL;

	key.name = name;

	if	(tab)
		return getvar(&key, tab, obj);

	if	((x = getvar(&key, LocalVar, obj ? obj : LocalObj)))
		return x;

	for (stack = VarStack; stack != NULL; stack = stack->next)
		if ((x = getvar(&key, stack->tab, obj ? obj : stack->obj)))
			return x;

	return NULL;
}


static EfiObj *vget (const EfiObj *base, void *data)
{
	return Var2Obj(data, base);	
}

void AddVar (EfiVarTab *tab, EfiVar *def, size_t dim)
{
	VarTabEntry entry, *ptr;

	tab = CurrentVarTab(tab);

	for (; dim-- > 0; def++)
	{
		if	(def->name == NULL)	continue;

		entry.name = def->name;
		entry.type = def->type;
		entry.desc = def->desc;
		entry.obj = NULL;
		entry.get = vget;
		entry.clean = rd_deref;
#if 1
		entry.data = def; /* !!!! */
#else
		entry.data = rd_refer(def); /* !!!! */
#endif
		ptr = vb_search(&tab->tab, &entry, var_cmp, VB_REPLACE);

		if	(ptr)
		{
			dbg_note(NULL, "[efmain:154]", "s", ptr->name);
			var_clean(ptr);
		}
	}
}

VarTabEntry *VarTab_xget (EfiVarTab *tab, const char *name)
{
	VarTabEntry entry, *ptr;

	entry.name = name;
	entry.desc = NULL;
	entry.type = NULL;
	entry.obj = NULL;
	entry.get = NULL;
	entry.clean = NULL;
	entry.data = NULL;

	tab = CurrentVarTab(tab);
	ptr = vb_search(&tab->tab, &entry, var_cmp, VB_ENTER);

	if	(ptr->name == name)
		ptr->name = mstrcpy(name);

	return ptr;
}

void VarTab_add (EfiVarTab *tab, VarTabEntry *entry)
{
	tab = CurrentVarTab(tab);
	var_clean(vb_search(&tab->tab, entry, var_cmp, VB_REPLACE));
}

void VarTab_xadd (EfiVarTab *tab, char *name, char *desc, EfiObj *obj)
{
	VarTabEntry entry, *ptr;

	entry.name = name;
	entry.desc = desc;
	entry.type = obj->type;
	entry.obj = obj;
	entry.get = NULL;
	entry.clean = memfree;
	entry.data = name;

	tab = CurrentVarTab(tab);
	ptr = vb_search(&tab->tab, &entry, var_cmp, VB_REPLACE);

	if	(ptr)
	{
		dbg_note(NULL, "[efmain:154]", "s", ptr->name);
		var_clean(ptr);
	}
}

void VarTab_del (EfiVarTab *tab, const char *name)
{
	if	(tab)
	{
		VarTabEntry *ptr = var_get(tab, name, VB_DELETE);

		if	(ptr && ptr->clean)
			ptr->clean(ptr->data);
	}
}
