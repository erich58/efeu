/*
Arbeiten mit stückweisen Polynomen

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


double pneval(Polynom *p, int dev, double x)
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
