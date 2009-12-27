/*	MDMAT - Achse generieren/freigeben
	(c) 1994 Erich Frühstück
*/


#include <EFEU/mdmat.h>

static ALLOCTAB(atab, 8, sizeof(mdaxis));


/*	Achse generieren
*/

mdaxis *new_axis(size_t dim)
{
	mdaxis *x;

	x = new_data(&atab);
	x->next = NULL;
	x->name = NULL;
	x->size = 0;
	x->dim = dim;
	x->idx = memalloc(dim * sizeof(mdindex));
	x->priv = NULL;
	x->flags = 0;
	return x;
}


/*	Achse freigeben
*/

void del_axis(mdaxis *axis)
{
	mdindex *p;
	size_t n;

	if	(axis == NULL)	return;

	del_axis(axis->next);

	for (p = axis->idx, n = axis->dim; n-- > 0; p++)
		memfree(p->name);

	memfree(axis->name);
	memfree(axis->idx);
	del_data(&atab, axis);
}


/*	Achse kopieren
*/

mdaxis *cpy_axis(mdaxis *old, unsigned mask)
{
	mdaxis *new;
	int i, dim;

	if	(old == NULL)	return NULL;

	for (i = dim = 0; i < old->dim; i++)
	{
		if	(old->idx[i].flags & mask)	continue;

		dim++;
	}

	new = new_axis(dim);
	new->name = mstrcpy(old->name);

	for (i = dim = 0; i < old->dim; i++)
	{
		if	(old->idx[i].flags & mask)	continue;

		new->idx[dim++].name = mstrcpy(old->idx[i].name);
	}

	return new;
}
