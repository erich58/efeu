/*
Spaltenhilsfunktionen
*/

#include <EFEU/Tabular.h>

TabularRow *Tabular_getrow (Tabular *tab, int n)
{
	if	(!tab)	return NULL;

	while (tab->row.used <= n)
	{
		TabularRow *row = vb_next(&tab->row);
		row->hline = TabularLinestyle_none;
		row->align = TabularAlign_left;
		row->top_margin = 0;
		row->height = 0;
	}

	return ((TabularRow *) tab->row.data) + n;
}

void Tabular_setrow (Tabular *tab, int n)
{
	if	(tab)
		tab->buf.row = *Tabular_getrow(tab, n);
}

