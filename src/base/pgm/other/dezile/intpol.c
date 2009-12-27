/*	Interepolationsfunktion zur Dezilberechnung
	(c) 1998 Erich Frühstück
	A-3423 St.Andrä/Wördern, Südtirolergasse 17-21/5

	Version 3.1
*/

#include "dezile.h"

#define	DIFF(x, i) (x[(i)+1] - x[i])	/* Differenzoperator */

pnom_t *intpol (size_t n, double *x, double *y)
{
	double y1, y2, y3, y4;
	double x0, x1, x2;
	double a, b;
	pnom_t *p;
	int i, k;
	
/*	Spezialfälle
*/
	if	(n == 0)	return NULL;
	if	(n == 1)	return pnconst(x[0], y[0]);
	if	(n == 2)	return pnconst(x[0], y[1] / x[1]);

/*	Speicherplatz generieren
*/
	n -= 2;
	p = pnalloc(2 * n + 1, 1);

/*	Koeffizienten berechnen
*/
	x1 = x[0];
	x2 = x[1];
	y1 = y2 = 0.;
	y3 = (n >= 0) ? DIFF(y, 0) / DIFF(x, 0) : y2;
	y4 = (n >= 1) ? DIFF(y, 1) / DIFF(x, 1) : y3;
	a = b = 0.;

	for (i = k = 0; i < n; i++)
	{
		double z;
		double g1, g2;

		x0 = x1;
		x1 = x2;
		x2 = x[i + 2];
		y1 = y2;
		y2 = y3;
		y3 = y4;
		y4 = (n >= i + 2) ? DIFF(y, i + 2) / DIFF(x, i + 2) : y3;

		a = b;
		g1 = (y2 - y1) / (x1 - x0);
		g2 = (y4 - y3) / (x2 - x1);
		b = (y3 * g1 + y2 * g2) / (g1 + g2);

		if	(b > a)
		{
			z = ((y2 - a) * x0 + (b - y2) * x1) / (b - a);
		}
		else	z = x1;

		if	(z > x0)
		{
			p->x[k] = x0;
			p->c[k][0] = a;
			p->c[k][1] = (y2 - a) / (z - x0);
			k++;
		}

		if	(z < x1)
		{
			p->x[k] = z;
			p->c[k][0] = y2;
			p->c[k][1] = (b - y2) / (x1 - z);
			k++;
		}
	}

	p->x[k] = x1;
	p->c[k][0] = y3;
	p->c[k][1] = 0.;
	p->dim = k + 1;
	return p;
}
