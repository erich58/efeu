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

static Obj_t *getvar(Var_t *var, VarTab_t *tab, const Obj_t *obj)
{
	if	(tab == NULL)	return NULL;

	if	(obj && mstrcmp(var->name, KEY_THIS) == 0)
		return RefObj(obj);

	var = xsearch(&tab->tab, var, XS_FIND);
	return var ? Var2Obj(var, obj) : NULL;	
}


Obj_t *GetVar (VarTab_t *tab, const char *name, const Obj_t *obj)
{
	Var_t key;
	Obj_t *x;
	VarStack_t *stack;

	if	(name == NULL)	return NULL;

	key.name = (char *) name;

	if	(tab)
		return getvar(&key, tab, obj);

	if	((x = getvar(&key, LocalVar, obj ? obj : LocalObj)))
		return x;

	for (stack = VarStack; stack != NULL; stack = stack->next)
		if ((x = getvar(&key, stack->tab, obj ? obj : stack->obj)))
			return x;

	return NULL;
}


void AddVar (VarTab_t *tab, Var_t *def, size_t dim)
{
	int i;
	Var_t *old;

	if	(tab == NULL)
	{
		if	(LocalVar == NULL)
			LocalVar = VarTab(NULL, 0);

		tab = LocalVar;
	}

	for (i = 0; i < dim; i++)
	{
		if	(def[i].name == NULL)	continue;

		if	((old = xsearch(&tab->tab, def + i, XS_REPLACE)))
		{
			DelVar(old);
			reg_cpy(1, def[i].name);
			errmsg(MSG_EFMAIN, 154);
		}
	}
}
