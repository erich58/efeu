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
		UnrefObj(var->obj);

		if	(var->entry_clean)
			var->entry_clean(var);
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

static void struct_clean (VarTabEntry *entry)
{
	rd_deref(entry->data);
}

void AddStruct (EfiVarTab *tab, EfiStruct *def, size_t dim)
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
		entry.entry_clean = struct_clean;
		entry.data = rd_refer(def);
		ptr = vb_search(&tab->tab, &entry, var_cmp, VB_REPLACE);

		if	(ptr)
		{
			log_note(NULL, "[efmain:154]", "s", ptr->name);
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
	entry.entry_clean = NULL;
	entry.data = NULL;

	tab = CurrentVarTab(tab);
	ptr = vb_search(&tab->tab, &entry, var_cmp, VB_ENTER);

	if	(ptr->name == name)
		ptr->name = mstrcpy(name);

	return ptr;
}

VarTabEntry *VarTab_next (EfiVarTab *tab)
{
	tab = CurrentVarTab(tab);
	return vb_next(&tab->tab);
}

void VarTab_qsort (EfiVarTab *tab)
{
	tab = CurrentVarTab(tab);
	vb_qsort(&tab->tab, var_cmp);
}

void VarTab_add (EfiVarTab *tab, VarTabEntry *entry)
{
	tab = CurrentVarTab(tab);
	var_clean(vb_search(&tab->tab, entry, var_cmp, VB_REPLACE));
}

static void xadd_clean (VarTabEntry *entry)
{
	memfree((char *) entry->name);
}

void VarTab_xadd (EfiVarTab *tab, char *name, char *desc, EfiObj *obj)
{
	VarTabEntry entry, *ptr;

	entry.name = name;
	entry.desc = desc;
	entry.type = obj ? obj->type : NULL;
	entry.obj = obj;
	entry.get = NULL;
	entry.entry_clean = xadd_clean;
	entry.data = NULL;

	tab = CurrentVarTab(tab);
	ptr = vb_search(&tab->tab, &entry, var_cmp, VB_REPLACE);

	if	(ptr)
	{
		log_note(NULL, "[efmain:154]", "s", ptr->name);
		var_clean(ptr);
	}
}

void VarTab_del (EfiVarTab *tab, const char *name)
{
	var_clean(var_get(tab, name, VB_DELETE));
}
