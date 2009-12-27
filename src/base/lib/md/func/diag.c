/*	Achsen einer Datenmatrix diagonalisieren
	(c) 1994 Erich Frühstück
*/

#include <EFEU/mdmat.h>

static void do_md_diag(EfiType *type, mdaxis *x2, mdaxis *x1, char *p2, char *p1)
{
	int i;
	size_t size;

	if	(x1 == NULL)
	{
		CopyData(type, p2, p1);
		return;
	}

	size = x2->size;

	if	(x1->flags & MDXFLAG_TEMP)
	{
		x2 = x2->next;
		size += x2->size;
	}

	x2 = x2->next;

	for (i = 0; i < x1->dim; i++)
	{
		do_md_diag(type, x2, x1->next, p2, p1);
		p2 += size;
		p1 += x1->size;
	}
}


mdmat *md_diag(mdmat *md, const char *def)
{
	mdmat *m2;
	mdaxis *x, **ptr;

	if	(md == NULL)	return NULL;

	md_setflag(md, def, 0, mdsf_mark, MDXFLAG_TEMP, NULL, 0);
	m2 = new_mdmat();
	m2->sbuf = NewStrPool();
	m2->axis = NULL;
	m2->type = md->type;
	m2->i_name = StrPool_copy(m2->sbuf, md->sbuf, md->i_name);
	m2->i_desc = StrPool_copy(m2->sbuf, md->sbuf, md->i_desc);

	ptr = &m2->axis;

	for (x = md->axis; x != NULL; x = x->next)
	{
		if	(x->flags & MDXFLAG_TEMP)
		{
			*ptr = cpy_axis(m2->sbuf, x, 0);
			ptr = &(*ptr)->next;
		}

		*ptr = cpy_axis(m2->sbuf, x, 0);
		ptr = &(*ptr)->next;
	}

	md_alloc(m2);
	memset(m2->data, 0, (size_t) m2->size);
	do_md_diag(m2->type, m2->axis, md->axis, m2->data, md->data);
	md_allflag(md, 0, mdsf_clear, MDXFLAG_TEMP, NULL, 0);
	return m2;
}
