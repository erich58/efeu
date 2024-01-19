/*	MDMAT - Datenstruktur kopieren
	(c) 1994 Erich Frühstück
*/

#include <EFEU/mdmat.h>

/*	Achse kopieren
*/

mdaxis *cpy_axis (StrPool *sbuf, mdaxis *old, unsigned skip_mask)
{
	mdaxis *new;
	int i, dim;

	if	(old == NULL)	return NULL;

	for (i = dim = 0; i < old->dim; i++)
	{
		if	(old->idx[i].flags & skip_mask)	continue;

		dim++;
	}

	new = new_axis(sbuf, dim);
	new->i_name = StrPool_copy(new->sbuf, old->sbuf, old->i_name);
	new->i_desc = StrPool_copy(new->sbuf, old->sbuf, old->i_desc);

	for (i = dim = 0; i < old->dim; i++)
	{
		if	(old->idx[i].flags & skip_mask)	continue;

		new->idx[dim].i_name = StrPool_copy(new->sbuf,
			old->sbuf, old->idx[i].i_name);
		new->idx[dim].i_desc = StrPool_copy(new->sbuf,
			old->sbuf, old->idx[i].i_desc);
		dim++;
	}

	return new;
}


static void do_copy (mdaxis *y, mdaxis *x, size_t size,
	char *py, char *px, unsigned mask)
{
	int i;

	if	(x == NULL)
	{
		memcpy(py, px, size);
		return;
	}

	for (i = 0; i < x->dim; i++)
	{
		if	(!(x->idx[i].flags & mask))
		{
			do_copy(y->next, x->next, size, py, px, mask);
			py += y->size;
		}

		px += x->size;
	}
}

mdmat *md_copy (mdmat *md, const char *def)
{
	mdmat *m2;
	mdaxis *x, **ptr;

	if	(md == NULL)	return NULL;

	md_setlock(md, def, 0, MDFLAG_TEMP);
	m2 = new_mdmat();
	m2->sbuf = NewStrPool();
	m2->axis = NULL;
	m2->type = md->type;

	ptr = &m2->axis;

	for (x = md->axis; x != NULL; x = x->next)
	{
		*ptr = cpy_axis(m2->sbuf, x, MDFLAG_TEMP);
		ptr = &(*ptr)->next;
	}

	md_alloc(m2);
	do_copy(m2->axis, md->axis, m2->type->size, m2->data, md->data, MDFLAG_TEMP);
	md_clrflag(md, 0, MDFLAG_TEMP);
	return m2;
}
