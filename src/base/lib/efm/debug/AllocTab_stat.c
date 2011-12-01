/*
Statistik der Speicherverwaltung generieren

$Copyright (C) 2009 Erich Frühstück
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

#include <EFEU/io.h>
#include <EFEU/Debug.h>

static void show_tab (AllocTab *tab, void *par)
{
	AllocTabList *x;
	size_t n;

	x = tab->blklist;

	for (n = 0; x != NULL; n++)
		x = x->next;

	io_printf(par, "%-12.12s %3zd byte", tab->name, tab->elsize);
	io_printf(par, " %5zd used %5zd free", tab->nused, tab->nfree);
	io_printf(par, " %3zd blocks/%5zd byte\n",
		n, tab->blksize * tab->elsize);
}

void AllocTab_stat (IO *out, AllocTab *tab, int verbosity)
{
	if	(!out)	return;

	if	(tab)
	{
		show_tab(tab, out);
	}
	else	AllocTab_walk(show_tab, out);
}
