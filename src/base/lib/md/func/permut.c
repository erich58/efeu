/*	Achsen permutierten
	(c) 1994 Erich Frühstück
*/

#include <EFEU/mdmat.h>
#include <EFEU/MatchPar.h>

typedef struct {
	unsigned oldpos;
	unsigned newpos;
	mdaxis *axis;
} XDATA;

#define	TEST(mp, x) \
	((x).newpos == 0 && MatchPar_exec(mp, \
	StrPool_get((x).axis->sbuf, (x).axis->i_name), (x).oldpos))

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
	MatchPar *match;
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
		match = MatchPar_create(list[i], mdim);

		for (j = 0; j < mdim; j++)
			if (TEST(match, tab[j])) tab[j].newpos = dim - i;

		rd_deref(match);
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
