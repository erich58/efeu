/*	Achsennamen auf Gleichheit testen
	(c) 1994 Erich Fr�hst�ck
*/

#include <EFEU/mdmat.h>

/*
Der Test erfolgt durch den Aufbau einer Suchtabelle f�r die Achsen,
wobei gez�hlt wird, wie oft eine Achse durch eine neue Definition
ersetzt wird. Die Suchtabelle wird anschlie�end gel�scht.
*/

int check_axis(mdaxis_t *axis)
{
	void *tab;
	int i, n;

	if	(axis == NULL || axis->dim == 0)
	{
		return 0;
	}

	tab = xcreate(axis->dim, skey_cmp);

	for (i = n = 0; i < axis->dim; i++)
	{
		if	(xsearch(tab, axis->idx + i, XS_REPLACE))
			n++;
	}

	xdestroy(tab, NULL);
	return n;
}
