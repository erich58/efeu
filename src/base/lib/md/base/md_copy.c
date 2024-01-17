/*	MDMAT - Datenstruktur kopieren
	(c) 1994 Erich Frühstück
*/

#include <EFEU/mdmat.h>

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

	unsigned mask = 0;

	if	(def)
	{
		mask = MDFLAG_TEMP;
		md_setflag(md, def, 0, NULL, 0, mdsf_lock, mask);
	}

	m2 = new_mdmat();
	m2->sbuf = NewStrPool();
	m2->axis = NULL;
	m2->type = md->type;

	ptr = &m2->axis;

	for (x = md->axis; x != NULL; x = x->next)
	{
		*ptr = cpy_axis(m2->sbuf, x, mask);
		ptr = &(*ptr)->next;
	}

	md_alloc(m2);
	do_copy(m2->axis, md->axis, m2->type->size, m2->data, md->data, mask);

	if	(mask)
		md_allflag(md, 0, NULL, 0, mdsf_clear, mask);

	return m2;
}
