/*	Arbeiten mit stückweisen Polynomen
	(c) 1991 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6

	Version 2.0
*/

#include <Math/pnom.h>


double pneval(pnom_t *p, int dev, double x)
{

	int i, j, k;
	double c, z;

	if	(dev < 0)	return 0.;
	if	(p == NULL)	return 0.;
	if	(p->dim == 0)	return 0.;
	if	(p->deg < dev)	return 0.;

/*	Suche nach den Intervall
*/
	for (k = p->dim - 1; k > 0; k--)
		if (x >= p->x[k]) break;

	x -= p->x[k];

/*	Berechnung des Funktionswertes (Ableitung)
	mit Horner Schema
*/
	z = 0.;

	for (i = p->deg; i >= dev; i--)
	{
		c = p->c[k][i];

		for (j = 0; j < dev; j++)
			c *= (i - j);

		z = z * x + c;
	}

	return z;
}
