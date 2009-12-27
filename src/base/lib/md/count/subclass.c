/*	Unterselektionsgruppe definieren
	(c) 1994 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6

	Version 2
*/

#include <EFEU/mdmat.h>
#include <EFEU/mdcount.h>

static int subclassify (MdSubClass_t *sel, const void *ptr);


/*	Unterselektion generieren
*/

MdSubClass_t *MdSubClass(MdClass_t *cdef)
{
	MdSubClass_t *sub;
	int i;

	sub = (MdSubClass_t *) memalloc((ulong_t)
		sizeof(MdSubClass_t) + cdef->dim * sizeof(int));
	sub->name = NULL;
	sub->dim = 0;
	sub->label = NULL;
	sub->classify = (MdClassify_t) subclassify;
	sub->idx = (int *) (sub + 1);
	sub->main = cdef;

	for (i = 0; i < cdef->dim; i++)
		sub->idx[i] = cdef->dim;

	return sub;
}


static int subclassify(MdSubClass_t *sel, const void *ptr)
{
	register int n = MdClassify(sel->main, ptr);

	if	(n >= 0 && n < sel->main->dim)
	{
		return sel->idx[n];
	}
	else	return sel->dim;
}
