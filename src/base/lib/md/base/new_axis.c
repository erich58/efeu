/*	MDMAT - Achse generieren/freigeben
	(c) 1994 Erich Frühstück
*/


#include <EFEU/mdmat.h>

static ALLOCTAB(atab, "mdaxis", 8, sizeof(mdaxis));


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

/*	Achsendimension bestimmen
*/

size_t md_dim(mdaxis *x)
{
	int n = 0;

	while (x)
	{
		x = x->next;
		++n;
	}

	return n;
}
