/*	Indexwert abtesten
	(c) 1995 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6

	Version 2.0
*/

#include <EFEU/efmain.h>

static ALLOCTAB(idxcmp_tab, 32, sizeof(idxcmp_t));

static int pcmp (idxcmp_t *t, const char *s, size_t n);
static int ncmp (idxcmp_t *t, const char *s, size_t n);
static int xcmp (idxcmp_t *t, const char *s, size_t n);


static int pcmp(idxcmp_t *tst, const char *name, size_t idx)
{
	return patcmp(tst->pattern, name, NULL);
}


static int ncmp(idxcmp_t *tst, const char *name, size_t idx)
{
	return (idx >= tst->minval && idx <= tst->maxval);
}


static int xcmp(idxcmp_t *tst, const char *name, size_t idx)
{
	return 1;
}


idxcmp_t *new_idxcmp(const char *def, size_t dim)
{
	idxcmp_t *x;;

	if	(def == NULL)	return NULL;

	x = new_data(&idxcmp_tab);
	memset(x, 0, sizeof(idxcmp_t));
	x->flag = 1;
	x->cmp = xcmp;

	switch (*def)
	{	
		case '-':	x->flag = 0; def++; break;
		case '+':	def++; break;
		default:	break;
	}

	if	(*def == '#')
	{
		long a;
		char *p;

		a = strtol(def + 1, &p, 0);

		if	(-a > (long) dim)	a = 0;
		else if	(a < 0)			a += 1 + (long) dim;

		x->minval = a;

		if	(*p == ':')
		{
			a = strtol(p + 1, NULL, 0);
		}
		else	a = x->minval;

		if	(-a > (long) dim)	a = 0;
		else if	(a <= 0)		a += 1 + (long) dim;

		x->maxval = a;
		x->cmp = ncmp;
	}
	else if	(*def != 0)
	{
		x->pattern = mstrcpy(def);
		x->cmp = pcmp;
	}

	return x;
}


idxcmp_t *idxcmplist(char **list, size_t ldim, size_t dim)
{
	idxcmp_t *test, **ptr;
	int i;

	test = NULL;
	ptr = &test;

	for (i = 0; i < ldim; i++)
	{
		*ptr = new_idxcmp(list[i], dim);
		ptr = &(*ptr)->next;
	}

	return test;
}


int idxcmp(idxcmp_t *test, const char *str, size_t idx)
{
	if	(test)
	{
		int flag;

		for (flag = !test->flag; test != NULL; test = test->next)
			if (test->cmp(test, str, idx)) flag = test->flag;

		return flag;
	}
	else	return 1;
}

void del_idxcmp(idxcmp_t *test)
{
	if	(test)
	{
		del_idxcmp(test->next);
		memfree(test->pattern);
		del_data(&idxcmp_tab, test);
	}
}
