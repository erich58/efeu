/*
Verwalten von Suchtabellen

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
