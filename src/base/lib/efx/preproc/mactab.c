/*	Makrotabellen
	(c) 1994 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6

	Version 0.4
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
