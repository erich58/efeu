/*
Bedingtes Integral

$Copyright (C) 1991 Erich Frühstück
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

double pnarea(Polynom *p, double a, double b)
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
