/*	Ableitung eines Polynoms
	(c) 1991 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6

	Version 2.0
*/

#include <Math/pnom.h>


/*	Ableitung eines Polynoms
*/

pnom_t *pndev(pnom_t *p0, int dev)
{
	pnom_t *p;
	int i, j, k;

	if	(dev < 0)	return NULL;
	if	(p0 == NULL)	return NULL;
	if	(p0->dim == 0)	return NULL;
	if	(p0->deg < dev)	return NULL;

	p = pnalloc(p0->dim, p0->deg - dev);

	for (i = 0; i < p->dim; ++i)
	{
		p->x[i] = p0->x[i];

		for (j = 0; j <= p->deg; ++j)
		{
			k = j + dev;
			p->c[i][j] = p0->c[i][k];

			while (k > j)
				p->c[i][j] *= k--;
		}
	}

	return p;
}
