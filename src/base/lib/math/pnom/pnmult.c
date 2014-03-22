/*
Multiplikation von stückweisen Polynomen

$Copyright (C) 1992 Erich Frühstück
This file is part of EFEU.

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Library General Public
License as published by the Free Software Foundation; either
version 2 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty
of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU Library General Public License for more details.

You should have received a copy of the GNU Library General Public
License along with this library; see the file COPYING.Library.
If not, write to the Free Software Foundation, Inc.,
59 Temple Place, Suite 330, Boston, MA 02111-1307, USA.
*/


#include <Math/pnom.h>


static void mul_sub (double c, double **a, size_t *deg, size_t n);

static Polynom *mul_pn;	/* Polynompointer */
static size_t mul_dim;	/* Polynomdimension */
static double mul_m;	/* Multiplikator */


/*	Koeffizienten berechnen
*/

static void mul_sub(double x, double **c, size_t *deg, size_t n)
{
	int i, imax;
	int j0, j, j1;

	imax = deg[0] + deg[1];

	for (i = 0; i <= imax; i++)
	{
		j0 = (i > deg[1] ? i - deg[1] : 0);
		j1 = (i < deg[0] ? i : deg[0]);
		mul_pn->c[mul_dim][i] = 0.;

		for (j = j0; j <= j1; j++)
			mul_pn->c[mul_dim][i] += c[0][j] * c[1][i-j];

		mul_pn->c[mul_dim][i] *= mul_m;
	}

	while (i <= mul_pn->deg)
		mul_pn->c[mul_dim][i++] = 0.;

	mul_pn->x[mul_dim++] = x;
}


/*	Polynome multiplizieren
*/

Polynom *pnmult(double c, Polynom *p1, Polynom *p2)
{
	Polynom *p[2];
	size_t deg;

	if	(c == 0. || p1 == NULL || p2 == NULL)
	{
		return NULL;
	}

	mul_m = c;
	p[0] = p1;
	p[1] = p2;
	deg = p1->deg + p2->deg;
	mul_dim = pnjoin(p, 2, NULL);
	mul_pn = pnalloc(mul_dim, deg);
	mul_dim = 0;
	(void) pnjoin(p, 2, mul_sub);
	return mul_pn;
}
