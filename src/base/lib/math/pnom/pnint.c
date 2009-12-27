/*	Stammfunktion eines stückweisen Polynoms
	(c) 1991 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6

	Version 2.0
*/

#include <Math/pnom.h>

pnom_t *pnint(pnom_t *p0, double x, double y)
{
	pnom_t *p;	/* Polynomstruktur */
	int i, j;	/* Hilfszähler */
	double z;	/* Hilfsvariable */

/*	Nullpolynom
*/
	if	(p0 == NULL || p0->dim == 0)
		return pnconst(x, y);

/*	Sonstiges Polynom
*/
	p = pnalloc(p0->dim, p0->deg + 1);

	for (i = 0; i < p->dim; i++)
	{
		p->x[i] = p0->x[i];
		p->c[i][0] = 0.;

		if	(i > 0)
		{
			z = p->x[i] - p->x[i-1];

			for (j = p->deg; j >= 0; j--)
				p->c[i][0] = p->c[i][0] * z
					+ p->c[i-1][j];
		}

		for (j = 1; j <= p->deg; j++)
			p->c[i][j] = p0->c[i][j - 1] / j;
	}

/*	Normierung
*/
	y -= pneval(p, 0, x);

	for (i = 0; i < p->dim; ++i)
		p->c[i][0] += y;

	return p;
}
