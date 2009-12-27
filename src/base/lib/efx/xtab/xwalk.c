/*	Durchwandern einer Suchtabelle
	(c) 1994 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6

	Version 0.4
*/

#include <EFEU/efmain.h>

int xwalk(xtab_t *tab, visit_t visit)

{
	xtab_t *xt;
	int i;

	if	(tab == NULL)	return 0;

	xt = (xtab_t *) tab;

	if	(visit != NULL)
	{
		for (i = 0; i < xt->dim; i++)
			if (!(*visit)(xt->tab[i])) break;

		return i;
	}

	return xt->dim;
}
