/*
Spaltenhilsfunktionen
*/

#include <EFEU/Tabular.h>

TabularCol *Tabular_getcol (Tabular *tab, int n)
{
	if	(!tab)	return NULL;

	while (tab->col.used <= n)
	{
		TabularCol *col = vb_next(&tab->col);
		col->align = TabularAlign_left;
		col->left_border = TabularLinestyle_none;
		col->left_margin = 0;
		col->right_margin = 0;
		col->right_border = TabularLinestyle_none;
		col->width = 0;
	}

	return ((TabularCol *) tab->col.data) + n;
}

void Tabular_setcol (Tabular *tab, int n)
{
	if	(tab)
		tab->buf.col = *Tabular_getcol(tab, n);
}
