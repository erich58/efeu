/*	Achsen permutierten
	(c) 1994 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6

	Version 2.0
*/

#include <EFEU/mdmat.h>

typedef struct {
	unsigned oldpos;
	unsigned newpos;
	mdaxis_t *axis;
} XDATA;

#define	TEST(test, x) ((x).newpos == 0 && mdtest(test, (x).axis->name, (x).oldpos))

static int cmp_data(const XDATA *a, const XDATA *b)
{
	if	(a->newpos < b->newpos)	return 1;
	else if	(a->newpos > b->newpos)	return -1;
	else if	(a->oldpos < b->oldpos)	return -1;
	else if	(a->oldpos > b->oldpos)	return 1;
	else				return	0;
}

void md_permut(mdmat_t *md, const char *str)
{
	char **list;
	size_t dim;
	size_t mdim;
	mdaxis_t *axis, **ptr;
	mdtest_t *test;
	XDATA *tab;
	int i, j;

	if	(str == NULL)	return;

	for (mdim = 0, axis = md->axis; axis != NULL; axis = axis->next)
		mdim++;

	if	(mdim <= 1)	return;

	dim = strsplit(str, "%s,;", &list);

	if	(dim == 0)	return;

	tab = ALLOC(mdim, XDATA);

	for (j = 0, axis = md->axis; axis != NULL; axis = axis->next, j++)
	{
		tab[j].newpos = 0;
		tab[j].oldpos = j + 1;
		tab[j].axis = axis;
	}

	for (i = 0; i < dim; i++)
	{
		test = new_test(list[i], mdim);

		for (j = 0; j < mdim; j++)
			if (TEST(test, tab[j])) tab[j].newpos = dim - i;

		del_test(test);
	}

	FREE(list);

	qsort(tab, mdim, sizeof(XDATA), (comp_t) cmp_data);

	ptr = &md->axis;

	for (j = 0; j < mdim; j++)
	{
		*ptr = tab[j].axis;
		ptr = &(*ptr)->next;
	}

	*ptr = NULL;
	FREE(tab);
}
