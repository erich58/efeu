/*
Stammfunktion eines stückweisen Polynoms

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
