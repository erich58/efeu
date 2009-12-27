/*	Achsen permutierten
	(c) 1994 Erich Frühstück
*/

#include <EFEU/mdtest.h>

typedef struct {
	unsigned oldpos;
	unsigned newpos;
	mdaxis *axis;
} XDATA;

#define	TEST(test, x) ((x).newpos == 0 && mdtest_eval(test, (x).axis->name, (x).oldpos))

static int cmp_xdata(const void *pa, const void *pb)
{
	const XDATA *a = pa;
	const XDATA *b = pb;

	if	(a->newpos < b->newpos)	return 1;
	else if	(a->newpos > b->newpos)	return -1;
	else if	(a->oldpos < b->oldpos)	return -1;
	else if	(a->oldpos > b->oldpos)	return 1;
	else				return	0;
}

void md_permut(mdmat *md, const char *str)
{
	char **list;
	size_t dim;
	size_t mdim;
	mdaxis *axis, **ptr;
	mdtest *test;
	XDATA *tab;
	int i, j;

	if	(str == NULL)	return;

	for (mdim = 0, axis = md->axis; axis != NULL; axis = axis->next)
		mdim++;

	if	(mdim <= 1)	return;

	dim = mstrsplit(str, "%s,;", &list);

	if	(dim == 0)	return;

	tab = memalloc(mdim * sizeof(XDATA));

	for (j = 0, axis = md->axis; axis != NULL; axis = axis->next, j++)
	{
		tab[j].newpos = 0;
		tab[j].oldpos = j + 1;
		tab[j].axis = axis;
	}

	for (i = 0; i < dim; i++)
	{
		test = mdtest_create(list[i], mdim);

		for (j = 0; j < mdim; j++)
			if (TEST(test, tab[j])) tab[j].newpos = dim - i;

		mdtest_clean(test);
	}

	memfree(list);

	qsort(tab, mdim, sizeof(XDATA), cmp_xdata);

	ptr = &md->axis;

	for (j = 0; j < mdim; j++)
	{
		*ptr = tab[j].axis;
		ptr = &(*ptr)->next;
	}

	*ptr = NULL;
	memfree(tab);
}
