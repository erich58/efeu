/*
Linearkombination von stückweisen Polynomen

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

#include <EFEU/efmain.h>
#include <Math/pnom.h>


static void lc_sub (double c, double **a, size_t *deg, size_t n);

static Polynom *lc_pn;	/* Polynompointer */
static size_t lc_dim;	/* Polynomdimension */
static double *lc_m;	/* Multiplikatoren */


/*	Polynome zusammenfassen
*/

static void lc_sub(double x, double **c, size_t *deg, size_t n)
{
	int i, j;

	for (j = 0; j <= lc_pn->deg; j++)
		lc_pn->c[lc_dim][j] = 0.;

	for (i = 0; i < n; i++)
	{
		for (j = 0; j <= deg[i]; j++)
		{
			lc_pn->c[lc_dim][j] += lc_m[i] * c[i][j];
		}
	}

	lc_pn->x[lc_dim++] = x;
}


/* VARARGS 1 */

Polynom *pnlincmb(Polynom *p, ...)
{
	Polynom *x;
	Polynom **pl;
	size_t n;
	size_t deg;
	va_list k;

/*	Bestimmung der Zahl der Polynome
*/
	va_start(k, p);

	for (n = 0, x = p; x != NULL; n++, x = va_arg(k, Polynom *))
		(void) va_arg(k, double);

	va_end(k);

	if	(n == 0)	return NULL;

/*	Vektoren aufbauen
*/
	lc_m = memalloc(n * sizeof(double));
	pl = memalloc(n * sizeof(Polynom *));
	va_start(k, p);
	deg = 0;

	for (n = 0, x = p; x != NULL; n++, x = va_arg(k, Polynom *))
	{
		lc_m[n] = va_arg(k, double);
		pl[n] = x;

		if	(deg < x->deg)	deg = x->deg;
	}

	va_end(k);

/*	Polynom generieren
*/
	lc_dim = pnjoin(pl, n, NULL);
	lc_pn = pnalloc(lc_dim, deg);
	lc_dim = 0;
	(void) pnjoin(pl, n, lc_sub);
	memfree(pl);
	memfree(lc_m);
	return lc_pn;
}
