/*	Achsenflags setzen/löschen
	(c) 1994 Erich Frühstück
*/

#include <EFEU/mdmat.h>

unsigned mdsf_mark(unsigned old, int sel, unsigned val)
{
	return sel ? (old | val) : (old & ~val);
}

unsigned mdsf_clear(unsigned old, int sel, unsigned val)
{
	return sel ? (old & ~val) : old;
}

unsigned mdsf_lock(unsigned old, int sel, unsigned val)
{
	return sel ? (old & ~val) : (old | val);
}

unsigned mdsf_toggle(unsigned old, int sel, unsigned val)
{
	if	(sel)
	{
		return (old & val) ? (old & ~val) : (old | val);
	}
	else	return old;
}

static void subfunc(mdaxis_t *x, mdlist_t *l, unsigned mask,
	mdsetflag_t func, unsigned flag);


static void allindex(mdaxis_t *x, unsigned mask, mdsetflag_t func, unsigned flag)
{
	int n;

	for (n = 0; n < x->dim; n++)
		if (!(x->idx[n].flags & mask))
			x->idx[n].flags = func(x->idx[n].flags, 1, flag);
}

static void subfunc(mdaxis_t *x, mdlist_t *l, unsigned mask,
	mdsetflag_t func, unsigned flag)
{
	mdtest_t *test;
	int n;

	test = mdtestlist(l->list, l->dim, x->dim);

	for (n = 0; n < x->dim; n++)
		if (!(x->idx[n].flags & mask))
			x->idx[n].flags = func(x->idx[n].flags,
				mdtest(test, x->idx[n].name, n + 1), flag);

	del_test(test);
}


void md_allflag(mdmat_t *md, unsigned mask,
	mdsetflag_t fx, unsigned vx, mdsetflag_t fi, unsigned vi)
{
	mdaxis_t *x;

	if	(md == NULL)	return;

	for (x = md->axis; x != NULL; x = x->next)
	{
		if	(fx)	x->flags = fx(x->flags, 1, vx);
		if	(fi)	allindex(x, mask, fi, vi);
	}
}


/*	Hauptfunktion
*/

void md_setflag(mdmat_t *md, const char *def, unsigned mask,
	mdsetflag_t fx, unsigned vx, mdsetflag_t fi, unsigned vi)
{
	mdlist_t *l, *list;
	mdaxis_t *x;
	mdtest_t *test;
	int n, dim;

	if	(md == NULL || md->axis == NULL)
		return;

/*	Achsenflags initialisieren
*/
	if	(fx)
	{
		for (x = md->axis; x != NULL; x = x->next)
			x->flags = fx(x->flags, 0, vx);
	}

/*	Universielle Selektion
*/
	if	(def == NULL)
	{
		if	(fi != 0)
		{
			for (x = md->axis; x != NULL; x = x->next)
				allindex(x, mask, fi, vi);
		}
		return;
	}

	dim = md_dim(md->axis);
	list = mdlist(def, 0);

	for (l = list; l != NULL; l = l->next)
	{
		test = new_test(l->name, dim);

		for (x = md->axis, n = 1; x != NULL; x = x->next, n++)
		{
			if	(mdtest(test, x->name, n))
			{
				if	(fx)	x->flags = fx(x->flags, 1, vx);

				if	(fi == NULL)	;
				else if	(l->dim)	subfunc(x, l, mask, fi, vi);
				else			allindex(x, mask, fi, vi);
			}
		}

		del_test(test);
	}

	del_mdlist(list);
}
