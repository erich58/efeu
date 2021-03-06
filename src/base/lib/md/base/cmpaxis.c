/*	Achsen vergleichen
	(c) 1994 Erich Frühstück
*/

#include <EFEU/mdmat.h>


int cmp_axis(mdaxis *x1, mdaxis *x2, int flag)
{
	size_t i, dim;
	int rflag;

	if	(x1 == x2)	return 0;

	if	(x1 == NULL || x2 == NULL)
	{
		return MDXCMP_ALL;
	}

	rflag = 0;

	if	((flag & MDXCMP_NAME) && StrPool_cmp(x1->sbuf, x1->i_name,
			x2->sbuf, x2->i_name) != 0)
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
			if	(StrPool_cmp(x1->sbuf, x1->idx[i].i_name,
					x2->sbuf, x2->idx[i].i_name) != 0)
			{
				rflag |= MDXCMP_IDX;
				break;
			}
		}
	}

	return rflag;
}
