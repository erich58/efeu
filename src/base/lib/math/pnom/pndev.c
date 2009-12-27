/*
Ableitung eines Polynoms

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


/*	Ableitung eines Polynoms
*/

Polynom *pndev(Polynom *p0, int dev)
{
	Polynom *p;
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
