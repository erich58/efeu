/*
Makrotabellen

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
#include <EFEU/preproc.h>
#include <EFEU/stack.h>

static ALLOCTAB(macdeftab, 0, sizeof(Macro_t));


Macro_t *NewMacro(void)
{
	Macro_t *mac;

	mac = new_data(&macdeftab);
	mac->name = NULL;
	mac->repl = NULL;
	mac->hasarg = 0;
	mac->vaarg = 0;
	mac->repl = NULL;
	mac->lock = 0;
	mac->dim = 0;
	mac->arg = NULL;
	mac->tab = NULL;
	return mac;
}


void DelMacro(Macro_t *mac)
{
	if	(mac != NULL)
	{
		memfree(mac->name);
		memfree(mac->repl);
		memfree(mac->arg);
		xdestroy(mac->tab, (clean_t) DelMacro);
		del_data(&macdeftab, mac);
	}
}


xtab_t *MacroTab = NULL;
static stack_t *MacroStack = NULL;


void PushMacroTab(xtab_t *tab)
{
	pushstack(&MacroStack, MacroTab);
	MacroTab = tab;
}


xtab_t *PopMacroTab(void)
{
	if	(MacroStack != NULL)
	{
		xtab_t *x = MacroTab;
		MacroTab = popstack(&MacroStack, NULL);
		return x;
	}
	else	return NULL;
}


Macro_t *GetMacro(const char *name)
{
	Macro_t key, *mac;
	stack_t *x;

	if	(name == NULL)	return NULL;

	key.name = (char *) name;

	if	((mac = xsearch(MacroTab, &key, XS_FIND)))
		return mac;

	if	(MacroStack == NULL)
		return NULL;

	for (x = MacroStack; x != NULL; x = x->next)
		if ((mac = xsearch(x->data, &key, XS_FIND)) != NULL)
			return mac;

	return NULL;
}


void AddMacDef(MacDef_t *def, size_t dim)
{
	int i;
	io_t *io;
	Macro_t *mac;

	for (i = 0; i < dim; i++)
	{
		io = io_cstr(def[i].def);
		mac = ParseMacro(io);
		io_close(io);

		if	(def[i].sub != NULL)
			mac->sub = def[i].sub;

		AddMacro(mac);
	}
}


void AddMacro(Macro_t *def)
{
	Macro_t *old;

	if	(MacroTab == NULL)
		MacroTab = NewMacroTab(0);

	if	((old = xsearch(MacroTab, def, XS_REPLACE)) != NULL)
	{
		DelMacro(old);
		reg_cpy(1, def->name);
		errmsg(PREPROC, 211);
	}
}
