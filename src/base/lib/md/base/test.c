/*	Teststruktur fuer Bezeichner generieren
	(c) 1994 Erich Frühstück
*/

#include <EFEU/mdmat.h>

static ALLOCTAB(test_tab, 32, sizeof(mdtest_t));

static int pcmp (mdtest_t *t, const char *s, size_t n);
static int ncmp (mdtest_t *t, const char *s, size_t n);
static int xcmp (mdtest_t *t, const char *s, size_t n);


static int pcmp(mdtest_t *tst, const char *name, size_t idx)
{
	return patcmp(tst->pattern, name, NULL);
}


static int ncmp(mdtest_t *tst, const char *name, size_t idx)
{
	return (idx >= tst->minval && idx <= tst->maxval);
}


static int xcmp(mdtest_t *tst, const char *name, size_t idx)
{
	return 1;
}


mdtest_t *new_test(const char *def, size_t dim)
{
	mdtest_t *x;;

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

mdtest_t *mdmktestlist(const char *list, size_t dim)
{
	mdtest_t *test, **ptr;
	io_t *io;
	char *p;

	io = io_cstr(list);
	test = NULL;
	ptr = &test;

	while (io_eat(io, "%s,") != EOF)
	{
		p = io_mgets(io, "%s,");
		*ptr = new_test(p, dim);
		memfree(p);
		ptr = &(*ptr)->next;
	}

	io_close(io);
	return test;
}

mdtest_t *mdtestlist(char **list, size_t ldim, size_t dim)
{
	mdtest_t *test, **ptr;
	int i;

	test = NULL;
	ptr = &test;

	for (i = 0; i < ldim; i++)
	{
		*ptr = new_test(list[i], dim);
		ptr = &(*ptr)->next;
	}

	return test;
}

int mdtest(mdtest_t *test, const char *str, size_t idx)
{
	int flag;

	if	(test == NULL)	return 0;

	flag = !test->flag;

	while (test != NULL)
	{
		if	(test->cmp(test, str, idx))
		{
			flag = test->flag;
		}

		test = test->next;
	}

	return flag;

	if	(test == NULL)
	{
		return 0;
	}
	else if	(test->cmp(test, str, idx))
	{
		return test->flag;
	}
	else	return !test->flag;
}

void del_test(mdtest_t *test)
{
	if	(test)
	{
		del_test(test->next);
		memfree(test->pattern);
		del_data(&test_tab, test);
	}
}
