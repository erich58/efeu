/*	Verwalten von Suchtabellen
	(c) 1994 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6

	Version 0.4
*/

#include <EFEU/efmain.h>

static ALLOCTAB(xtab_tab, 0, sizeof(xtab_t));


/*	Suchtabelle generieren
*/

xtab_t *xcreate(size_t size, comp_t comp)
{
	xtab_t *x;

	x = new_data(&xtab_tab);
	x->bsize = size;
	x->dim = 0;
	x->comp = comp;
	x->tsize = 0;
	x->tab = NULL;
	return x;
}


/*	Suchtabelle löschen
*/

void xdestroy(xtab_t *tab, clean_t action)
{
	int i;

	if	(tab == NULL)	return;

	if	(action != NULL)
	{
		for (i = 0; i < tab->dim; i++)
			(*action)(tab->tab[i]);
	}

	lfree((void *) tab->tab);
	tab->tsize = tab->dim = 0;
	tab->tab = NULL;
	del_data(&xtab_tab, tab);
}
