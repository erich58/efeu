/*	Achsen vergleichen
	(c) 1994 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6

	Version 2.0
*/

#include <EFEU/mdmat.h>


int cmp_axis(mdaxis_t *x1, mdaxis_t *x2, int flag)
{
	size_t i, dim;
	int rflag;

	if	(x1 == x2)	return 0;

	if	(x1 == NULL || x2 == NULL)
	{
		return MDXCMP_ALL;
	}

	rflag = 0;

	if	((flag & MDXCMP_NAME) && mstrcmp(x1->name, x2->name) != 0)
	{
		rflag |= MDXCMP_NAME;
	}

	if	((flag & MDXCMP_DIM) && x2->dim != x1->dim)
	{
		rflag |= MDXCMP_DIM;
	}

	if	(flag & MDXCMP_IDX)
	{
		dim = min(x1->dim, x2->dim);

		for (i = 0; i < dim; i++)
		{
			if	(mstrcmp(x1->idx[i].name, x2->idx[i].name) != 0)
			{
				rflag |= MDXCMP_IDX;
				break;
			}
		}
	}

	return rflag;
}
