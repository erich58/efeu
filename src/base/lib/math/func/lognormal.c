/*
:*:	lognormal distribution
:de:	Lognormalverteilung

$Name dlognormal, plognormal, qlognormal

$Copyright (C) 2002 Erich Frühstück
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

#include <Math/func.h>

/*
:*:
The function |$1| computes the density of the lognormal distribution.
:de:
Die Funktion |$1| berechnet die Dichte der Lognormalverteilung.
*/

double dlognormal (double q, double mu, double sigma)
{
	double x;

	if	(sigma <= 0.)
		return ExceptionValue(0.);

	x = (log(q) - mu) / sigma;
	return M_1_SQRT_2PI * exp(-0.5 * x * x) / (q * sigma);
}

/*
:*:
The function |$1| computes the cummulative probability of the
lognormal distribution.
:de:
Die Funktion |$1| berechnet die kummulative Häufigkeit der Lognormalverteilung.
*/

double plognormal (double q, double mu, double sigma)
{
	if	(sigma <= 0.)
		return ExceptionValue(0.);
    
	return (q > 0.) ? pnormal(log(q), mu, sigma) : 0.;
}

/*
:*:
The function |$1| computes the quantil function of the lognormal distribution.
:de:
Die Funktion |$1| berechnet die Inverse der Lognormalverteilung.
*/

double qlognormal (double p, double mu, double sigma)
{
	if	(p >= 1.)	return ExceptionValue(1.);
	if	(p > 0)		return exp(qnormal(p, mu, sigma));

	return 0.;
}
