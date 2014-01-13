/*	Funktion mit mdmat - Argumenten aufrufen
	(c) 1995 Erich Frühstück
*/

#include <EFEU/mdmat.h>

static mdaxis *make_axis(StrPool *sbuf, mdaxis *x1, mdaxis *x2, unsigned mask)
{
	mdaxis *x, **ptr;

	x = NULL;
	ptr = &x;

	while (x1 && x2)
	{
		if	(cmp_axis(x1, x2, MDXCMP_DIM) != 0)
		{
			log_note(NULL, "[mdmat:92]", "ss",
				StrPool_get(x1->sbuf, x1->i_name),
				StrPool_get(x2->sbuf, x2->i_name));
			*ptr = cpy_axis(sbuf, x1->dim >= x2->dim ? x1 : x2, mask);
		}
		else	*ptr = cpy_axis(sbuf, x1, mask);

		ptr = &(*ptr)->next;
		x1 = x1->next;
		x2 = x2->next;
	}

	while (x1)
	{
		*ptr = cpy_axis(sbuf, x1, mask);
		ptr = &(*ptr)->next;
		x1 = x1->next;
	}

	while (x2)
	{
		*ptr = cpy_axis(sbuf, x2, mask);
		ptr = &(*ptr)->next;
		x2 = x2->next;
	}

	return x;
}


static void do_md_term (EfiFunc *func, mdaxis *x, char *p, mdaxis *x1,
	char *p1, mdaxis *x2, char *p2)
{
	int i, n;
	size_t s, s1, s2;

	if	(x == NULL)
	{
		if	(p != NULL)
		{
			CallFunc(func->type, p, func, p1, p2);
		}
		else	CallVoidFunc(func, p1, p2);

		return;
	}

	n = x->dim;
	s = p ? x->size : 0;
	x = x->next;

	if	(x1)
	{
		if	(x1->dim < n)	n = x1->dim;

		s1 = x1->size;
		x1 = x1->next;
	}
	else	s1 = 0;

	if	(x2)
	{
		if	(x2->dim < n)	n = x2->dim;

		s2 = x2->size;
		x2 = x2->next;
	}
	else	s2 = 0;

	for (i = 0; i < n; i++)
	{
		do_md_term(func, x, p, x1, p1, x2, p2);
		p += s;
		p1 += s1;
		p2 += s2;
	}
}


mdmat *md_assign (const char *name, mdmat *m1, mdmat *m2)
{
	EfiVirFunc *virfunc;
	EfiFunc *func;

	if	(m1 == NULL)	return NULL;

	virfunc = GetTypeFunc(m1->type, name);

	if	(m2)
	{
		func = GetFunc(NULL, virfunc, 2, m1->type, 1, m2->type, 0);
	}
	else	func = GetFunc(NULL, virfunc, 1, m1->type, 1);

	if	(func == NULL)
	{
		log_note(NULL, "[mdmat:94]", "s", name); 
	}
	else if	(m2)
	{
		do_md_term(func, m1->axis, NULL, m1->axis, m1->data, m2->axis, m2->data);
	}
	else	do_md_term(func, m1->axis, NULL, m1->axis, m1->data, NULL, NULL);

	return rd_refer(m1);
}


mdmat *md_term (EfiVirFunc *virfunc, mdmat *m1, mdmat *m2)
{
	EfiFunc *func;
	mdmat *md;

	if	(m1 == NULL)
	{
		return NULL;
	}
	else if	(m2 == NULL)
	{
		func = GetFunc(NULL, virfunc, 1, m1->type, 0);
	}
	else	func = GetFunc(NULL, virfunc, 2, m1->type, 0, m2->type, 0);

	if	(func == NULL)
	{
		log_note(NULL, "[mdmat:91]", NULL); 
		return rd_refer(m1);
	}

	if	(func->type == NULL)
	{
		log_note(NULL, "[mdmat:93]", "s", func->name); 
		return rd_refer(m1);
	}

	md = new_mdmat();
	md->sbuf = NewStrPool();
	md->type = (EfiType *) func->type;
	md->axis = make_axis(md->sbuf, m1->axis, m2 ? m2->axis : NULL, 0);
	md_alloc(md);
	memset(md->data, 0, md->size);

	if	(m2)
	{
		do_md_term(func, md->axis, md->data, m1->axis, m1->data, m2->axis, m2->data);
	}
	else	do_md_term(func, md->axis, md->data, m1->axis, m1->data, NULL, NULL);

	return md;
}
