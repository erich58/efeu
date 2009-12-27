/*	Achsenflags setzen/löschen
	(c) 1994 Erich Frühstück
*/

#include <EFEU/mdtest.h>

unsigned mdsf_mark (unsigned old, int sel, unsigned val)
{
	return sel ? (old | val) : (old & ~val);
}

unsigned mdsf_clear (unsigned old, int sel, unsigned val)
{
	return sel ? (old & ~val) : old;
}

unsigned mdsf_lock (unsigned old, int sel, unsigned val)
{
	return sel ? (old & ~val) : (old | val);
}

unsigned mdsf_toggle (unsigned old, int sel, unsigned val)
{
	if	(sel)
	{
		return (old & val) ? (old & ~val) : (old | val);
	}
	else	return old;
}

static void subfunc (mdaxis *x, mdlist *l, unsigned mask,
	unsigned (*func) (unsigned old, int flag, unsigned val), unsigned flag);


static void allindex (mdaxis *x, unsigned mask,
	unsigned (*func) (unsigned old, int flag, unsigned val), unsigned flag)
{
	int n;

	for (n = 0; n < x->dim; n++)
		if (!(x->idx[n].flags & mask))
			x->idx[n].flags = func(x->idx[n].flags, 1, flag);
}

static void subfunc (mdaxis *x, mdlist *l, unsigned mask,
	unsigned (*func) (unsigned old, int flag, unsigned val), unsigned flag)
{
	mdtest *test;
	int n;

	test = mdtestlist(l->list, l->dim, x->dim);

	for (n = 0; n < x->dim; n++)
		if (!(x->idx[n].flags & mask))
			x->idx[n].flags = func(x->idx[n].flags,
				mdtest_eval(test, x->idx[n].name, n + 1), flag);

	mdtest_clean(test);
}


void md_allflag (mdmat *md, unsigned mask,
	unsigned (*fx) (unsigned old, int flag, unsigned val), unsigned vx,
	unsigned (*fi) (unsigned old, int flag, unsigned val), unsigned vi)
{
	mdaxis *x;

	if	(md == NULL)	return;

	for (x = md->axis; x != NULL; x = x->next)
	{
		if	(fx)	x->flags = fx(x->flags, 1, vx);
		if	(fi)	allindex(x, mask, fi, vi);
	}
}


/*	Hauptfunktion
*/

void md_setflag (mdmat *md, const char *def, unsigned mask,
	unsigned (*fx) (unsigned old, int flag, unsigned val), unsigned vx,
	unsigned (*fi) (unsigned old, int flag, unsigned val), unsigned vi)
{
	mdlist *l, *list;
	mdaxis *x;
	mdtest *test;
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
	list = str2mdlist(def, 0);

	for (l = list; l != NULL; l = l->next)
	{
		test = mdtest_create(l->name, dim);

		for (x = md->axis, n = 1; x != NULL; x = x->next, n++)
		{
			if	(mdtest_eval(test, x->name, n))
			{
				if	(fx)	x->flags = fx(x->flags, 1, vx);

				if	(fi == NULL)
					;
				else if	(l->dim)
					subfunc(x, l, mask, fi, vi);
				else
					allindex(x, mask, fi, vi);
			}
		}

		mdtest_clean(test);
	}

	del_mdlist(list);
}
