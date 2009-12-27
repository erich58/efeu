/*
Zufallszahlenfunktionen

$Copyright (C) 2001 Erich Frühstück
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

#include <EFEU/Random.h>
#include <math.h>

#ifndef	M_PI
#define M_PI        3.14159265358979323846264338327950288
#endif

size_t RandomIndex (Random *rd, size_t dim)
{
	size_t k = dim * UniformRandom(rd);
	return (k < dim) ? k : dim - 1;
}

double LinearRandom (Random *rd, double z)
{
	double x = UniformRandom(rd);

	if	(z == 0.)	return x;
	if	(z >= 1.)	return sqrt(x);
	if	(z <= -1.)	return 1. - sqrt(1. - x);

	return 0.5 * (z - 1. + sqrt(z * (z - 2. + 4. * x) + 1.)) / z;
}

double NormalRandom (Random *rd)
{
	double x;

	x = sqrt(-2. * log(UniformRandom(rd)));
	x *= cos(M_PI * 2. * UniformRandom(rd));
	return x;
}

/*	Poissonverteilung: bei großem Mittelwert wird Normalverteilung
	verwendet. Ab 9 annähernd Normalverteilt, ab 700 Überlauf.
*/

#define	POISSON_MAX	20.5 	/* 9 <= x <= 700 */

int PoissonRandom (Random *rd, double mw)
{
	double x, lim;
	int n;

	if	(mw > POISSON_MAX)
		return (int) (mw + sqrt(mw) * NormalRandom(rd) + 0.5);

	lim = exp(-mw);

	for (n = 0, x = 1.; x > lim; n++)
		x *= UniformRandom(rd);

	return n ? n - 1 : 0;
}


/*	Zufallsrundung
*/

int RandomRound (Random *rd, double x)
{
	int val;
	double rval;

	val = (int) x;
	x -= val;
	rval = UniformRandom(rd);

	if	(x >= 0.)
	{
		if	(x > rval)	val++;
	}
	else	if	(-x > rval)	val--;

	return val;
}
