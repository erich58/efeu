/*	MDMAT - Achse generieren/freigeben
	(c) 1994 Erich Frühstück
*/


#include <EFEU/mdmat.h>

static ALLOCTAB(atab, 8, sizeof(mdaxis));


/*	Achse generieren
*/

mdaxis *new_axis (StrPool *sbuf, size_t dim)
{
	mdaxis *x;

	x = new_data(&atab);
	x->next = NULL;
	x->sbuf = rd_refer(sbuf);
	x->i_name = 0;
	x->i_desc = 0;
	x->size = 0;
	x->dim = dim;
	x->idx = memalloc(dim * sizeof(mdindex));
	x->priv = NULL;
	x->lbl = NULL;
	x->flags = 0;
	return x;
}


/*	Achse freigeben
*/

void del_axis(mdaxis *axis)
{
	if	(axis == NULL)	return;

	del_axis(axis->next);
	rd_deref(axis->sbuf);
	rd_deref(axis->lbl);
	memfree(axis->idx);
	del_data(&atab, axis);
}


/*	Achse kopieren
*/

mdaxis *cpy_axis (StrPool *sbuf, mdaxis *old, unsigned mask)
{
	mdaxis *new;
	int i, dim;

	if	(old == NULL)	return NULL;

	for (i = dim = 0; i < old->dim; i++)
	{
		if	(old->idx[i].flags & mask)	continue;

		dim++;
	}

	new = new_axis(sbuf, dim);
	new->i_name = StrPool_copy(new->sbuf, old->sbuf, old->i_name);
	new->i_desc = StrPool_copy(new->sbuf, old->sbuf, old->i_desc);

	for (i = dim = 0; i < old->dim; i++)
	{
		if	(old->idx[i].flags & mask)	continue;

		new->idx[dim].i_name = StrPool_copy(new->sbuf,
			old->sbuf, old->idx[i].i_name);
		new->idx[dim].i_desc = StrPool_copy(new->sbuf,
			old->sbuf, old->idx[i].i_desc);
		dim++;
	}

	return new;
}
