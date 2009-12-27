/*	Variablentabellen
	(c) 1994 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6

	Version 0.4
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

REFTYPE(VarTabRefType, "VarTab", vtab_ident, vtab_admin);

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
