/*
Variablentabellen

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

static ALLOCTAB(tab_vtab, "EfiVarTab", 0, sizeof(EfiVarTab));

static char *vtab_ident (const void *data)
{
	const EfiVarTab *tab = data;

	if	(tab->name)
		return msprintf("%s[%d]", tab->name, tab->tab.used);

	return msprintf("%p[%d]", tab, tab->tab.used);
}

static void vtab_subclean (void *data)
{
	VarTabEntry *var = data;
	UnrefObj(var->obj);

	if	(var->entry_clean)
		var->entry_clean(var);
}

static void vtab_clean (void *data)
{
	EfiVarTab *tg = data;
	memfree(tg->name);
	vb_clean(&tg->tab, vtab_subclean);
	vb_free(&tg->tab);
	del_data(&tab_vtab, tg);
}

static RefType VarTabRefType = REFTYPE_INIT("VarTab",
	vtab_ident, vtab_clean);

#define	VTABSIZE	32

EfiVarTab *VarTab (const char *name, size_t size)
{
	EfiVarTab *x = new_data(&tab_vtab);
	x->name = mstrcpy(name);
	vb_init(&x->tab, size ? size : VTABSIZE, sizeof(VarTabEntry));
	return rd_init(&VarTabRefType, x);
}

EfiVarTab *CurrentVarTab (EfiVarTab *tab)
{
	if	(tab)	return tab;

	if	(LocalVar == NULL)
		LocalVar = VarTab(NULL, 0);

	return LocalVar;
}
