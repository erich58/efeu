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

static ALLOCTAB(macdeftab, 0, sizeof(PPMacro));


PPMacro *NewMacro (void)
{
	PPMacro *mac;

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


void DelMacro (void *ptr)
{
	PPMacro *mac = ptr;

	if	(mac != NULL)
	{
		memfree(mac->name);
		memfree(mac->repl);
		memfree(mac->arg);
		rd_deref(mac->tab);
		del_data(&macdeftab, mac);
	}
}


NameKeyTab *MacroTab = NULL;
static Stack *MacroStack = NULL;


void PushMacroTab (NameKeyTab *tab)
{
	pushstack(&MacroStack, MacroTab);
	MacroTab = tab;
}


NameKeyTab *PopMacroTab (void)
{
	if	(MacroStack != NULL)
	{
		NameKeyTab *x = MacroTab;
		MacroTab = popstack(&MacroStack, NULL);
		return x;
	}
	else	return NULL;
}


PPMacro *GetMacro (const char *name)
{
	PPMacro *mac;
	Stack *x;

	if	(name == NULL)	return NULL;

	if	((mac = nkt_fetch(MacroTab, name, NULL)))
		return mac;

	if	(MacroStack == NULL)
		return NULL;

	for (x = MacroStack; x != NULL; x = x->next)
		if ((mac = nkt_fetch(x->data, name, NULL)) != NULL)
			return mac;

	return NULL;
}


void AddMacDef (PPMacDef *def, size_t dim)
{
	int i;
	IO *io;
	PPMacro *mac;

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


void AddMacro (PPMacro *def)
{
	if	(MacroTab == NULL)
		MacroTab = NewMacroTab(0);

	if	(nkt_fetch(MacroTab, def->name, NULL))
		dbg_note(NULL, "[preproc:211]", "s", def->name);

	nkt_insert(MacroTab, def->name, def);
}

void ShowMacro (IO *io, PPMacro *mac)
{
	if	(!mac)	return;

	io_puts(mac->name, io);

	if	(mac->hasarg)
		io_printf(io, "[%d%s]", mac->dim, mac->vaarg ? "v" : "");

	io_printf(io, " %#s\n", mac->repl);

	if	(mac->dim)
	{
		IO *sub;
		int i;

		io_puts("{\n", io);
		sub = io_lmark(rd_refer(io), "\t", NULL, 0);

		for (i = 0; i < mac->dim; i++)
			ShowMacro(sub, mac->arg[i]);

		rd_deref(sub);
		io_puts("}\n", io);
	}
}
