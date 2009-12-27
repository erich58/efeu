/*	Teststruktur fuer Bezeichner generieren
	(c) 1994 Erich Frühstück
*/

#include <EFEU/mdtest.h>

static ALLOCTAB(test_tab, 32, sizeof(mdtest));

static int pcmp (mdtest *t, const char *s, size_t n);
static int ncmp (mdtest *t, const char *s, size_t n);
static int xcmp (mdtest *t, const char *s, size_t n);


static int pcmp(mdtest *tst, const char *name, size_t idx)
{
	return patcmp(tst->pattern, name, NULL);
}


static int ncmp(mdtest *tst, const char *name, size_t idx)
{
	return (idx >= tst->minval && idx <= tst->maxval);
}


static int xcmp(mdtest *tst, const char *name, size_t idx)
{
	return 1;
}


mdtest *mdtest_create(const char *def, size_t dim)
{
	mdtest *x;;

	x = new_data(&test_tab);
	x->pattern = NULL;
	x->next = NULL;
	x->flag = 1;
	x->cmp = xcmp;

	if	(def == NULL)
	{
		return x;
	}

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

mdtest *mdmktestlist(const char *list, size_t dim)
{
	mdtest *test, **ptr;
	IO *io;
	char *p;

	io = io_cstr(list);
	test = NULL;
	ptr = &test;

	while (io_eat(io, "%s,") != EOF)
	{
		p = io_mgets(io, "%s,");
		*ptr = mdtest_create(p, dim);
		memfree(p);
		ptr = &(*ptr)->next;
	}

	io_close(io);
	return test;
}

mdtest *mdtestlist(char **list, size_t ldim, size_t dim)
{
	mdtest *test, **ptr;
	int i;

	test = NULL;
	ptr = &test;

	for (i = 0; i < ldim; i++)
	{
		*ptr = mdtest_create(list[i], dim);
		ptr = &(*ptr)->next;
	}

	return test;
}

int mdtest_eval (mdtest *test, const char *str, size_t idx)
{
	int flag;

	if	(test == NULL)	return 0;

	flag = !test->flag;

	while (test != NULL)
	{
		if	(test->cmp(test, str, idx))
			flag = test->flag;

		test = test->next;
	}

	return flag;
}

void mdtest_clean (mdtest *test)
{
	if	(test)
	{
		mdtest_clean(test->next);
		memfree(test->pattern);
		del_data(&test_tab, test);
	}
}
