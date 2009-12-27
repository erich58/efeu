/*	Achsen einer Datenmatrix aufsummieren
	(c) 1994 Erich Frühstück
*/

#include <EFEU/mdmat.h>

static void do_md_sum(Func_t *func, mdaxis_t *x2, mdaxis_t *x, char *base, char *data)
{
	int i;

	if	(x == NULL)
	{
		CallVoidFunc(func, base, data);
	}
	else if	(x->flags & MDXFLAG_TEMP)
	{
		for (i = 0; i < x->dim; i++)
		{
			do_md_sum(func, x2, x->next, base, data);
			data += x->size;
		}
	}
	else
	{
		for (i = 0; i < x->dim; i++)
		{
			do_md_sum(func, x2->next, x->next, base, data);
			base += x2->size;
			data += x->size;
		}
	}
}


mdmat_t *md_sum(mdmat_t *md, const char *def)
{
	mdmat_t *m2;
	Func_t *add;
	mdaxis_t *x, **ptr;

	if	(md == NULL)	return NULL;

	add = mdfunc_add(md->type);

	if	(add == NULL)	return NULL;

	md_setflag(md, def, 0, mdsf_mark, MDXFLAG_TEMP, NULL, 0);
	m2 = new_mdmat();
	m2->axis = NULL;
	m2->type = md->type;

	ptr = &m2->axis;

	for (x = md->axis; x != NULL; x = x->next)
	{
		if	(!(x->flags & MDXFLAG_TEMP))
		{
			*ptr = cpy_axis(x, 0);
			ptr = &(*ptr)->next;
		}
	}

	m2->size = md_size(m2->axis, m2->type->size);
	m2->data = memalloc(m2->size);
	memset(m2->data, 0, (size_t) m2->size);
	do_md_sum(add, m2->axis, md->axis, m2->data, md->data);
	md_allflag(md, 0, mdsf_clear, MDXFLAG_TEMP, NULL, 0);
	return m2;
}
