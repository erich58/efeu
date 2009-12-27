/*	Arbeiten mit stückweisen Polynomen
	Bedingtes Integral
	(c) 1991 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6

	Version 2.0
*/

#include <Math/pnom.h>


static double sub (double x, double *c, size_t n);


/*	Hilfsroutine
*/

static double sub(double x, double *c, size_t n)
{
	double z;

	z = c[n] / (n + 1.);

	while (n-- > 0)
		z = z * x + c[n] / (n + 1.);

	return z * x;
}


/*	Integrieren
*/

double pnarea(pnom_t *p, double a, double b)
{

	int k1, k2;
	double z;

	if	(p == NULL)	return 0.;
	if	(p->dim == 0)	return 0.;

	for (k2 = p->dim - 1; k2 > 0; k2--)
		if (b >= p->x[k2]) break;

	for (k1 = k2; k1 > 0; k1--)
		if (a >= p->x[k1]) break;

	z = sub(b - p->x[k2], p->c[k2], p->deg);
	z -= sub(a - p->x[k1], p->c[k1], p->deg);

	for (; k1 < k2; k1++)
		z += sub(p->x[k1 + 1] - p->x[k1], p->c[k1], p->deg);

	return z;
}
