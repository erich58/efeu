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

static ALLOCTAB(tab_vtab, 0, sizeof(VarTab_t));

static char *vtab_ident(VarTab_t *tab)
{
	if	(tab->name)
		return msprintf("%s[%d]", tab->name, tab->tab.dim);

	return msprintf("%p[%d]", tab, tab->tab.dim);
}

static VarTab_t *vtab_admin (VarTab_t *tg, const VarTab_t *src)
{
	if	(tg)
	{
		memfree(tg->name);
		xdestroy(&tg->tab, (clean_t) DelVar);
		del_data(&tab_vtab, tg);
		return NULL;
	}
	else	return new_data(&tab_vtab);
}

ADMINREFTYPE(VarTabRefType, "VarTab", vtab_ident, vtab_admin);

#define	VTABSIZE	32

VarTab_t *VarTab(const char *name, size_t size)
{
	VarTab_t *x = rd_create(&VarTabRefType);
	x->name = mstrcpy(name);
	x->tab.comp = skey_cmp;
	x->tab.tab = NULL;
	x->tab.dim = 0;
	x->tab.tsize = 0;
	x->tab.bsize = size ? size : VTABSIZE;
	return x;
}
