/*
Normalverteilte Zufallszahlen

$Copyright (C) 1994 Erich Frühstück
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


#include <EFEU/efutil.h>
#include <EFEU/rand48.h>
#include <math.h>

#ifndef	M_PI
#define M_PI        3.14159265358979323846264338327950288
#endif


double NormalRand48(Rand48_t *rand, int dim)
{
	double x;
	int i, n;

	if	(dim <= 0)
	{
		x = sqrt(-2. * log(Rand48(rand)));
		x *= cos(M_PI * 2. * Rand48(rand));
		return x;
	}

	n = 3 * dim * dim;
	x = 0.;

	for (i = 0; i < n; i++)
		x += Rand48(rand);

	return (2. * x - (double) n) / (double) dim;
}
