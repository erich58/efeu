/*
:*:	normal distribution
:de:	Funktionen zur Normalverteilung

$Name dnormal, pnormal, qnormal

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
The function |$1| computes the density of the normal distribution.
:de:
Die Funktion |$1| berechnet die Dichte der Normalverteilung.
*/

double dnormal (double q, double mu, double sigma)
{
	if	(sigma <= 0.)
		return ExceptionValue(0.);

	q = (q - mu) / sigma;
	return M_1_SQRT_2PI * exp(-0.5 * q * q) / sigma;
}

/*
:*:
The function |$1| computes the cummulative probability of the
normal distribution.
:de:
Die Funktion |$1| berechnet die kummulative Häufigkeit der Normalverteilung.
*/

double pnormal (double q, double mu, double sigma)
{
	if	(sigma <= 0.)
		return ExceptionValue(0.);
    
	q = (q - mu) / (sigma * M_SQRT2);

	if	(q >= 0.)
		return 0.5 + 0.5 * erf(q);

	return 0.5 * erfc(-q);
}

/*
:*:
The function |$1| computes the quantil function of the normal distribution.
:de:
Die Funktion |$1| berechnet die Inverse der Normalverteilung.
*/

double qnormal (double p, double mu, double sigma)
{
	double q, r, val;

	if	(p <= 0.)
	{
		return ExceptionValue(-1.);
	}
	else if	(p <= 0.08)
	{
		r = sqrt(-log(p));
		val = -(((2.32121276858 * r + 4.85014127135) * r
			- 2.29796479134) * r - 2.78718931138)
			/ ((1.63706781897 * r + 3.54388924762) * r + 1.0);
	}
	else if	(p < 0.92)
	{
		q = p - 0.5;
		r = q * q;
		val = q * (((-25.44106049637 * r + 41.39119773534) * r
			- 18.61500062529) * r + 2.50662823884)
			/ ((((3.13082909833 * r - 21.06224101826) * r
			+ 23.08336743743) * r + -8.47351093090) * r + 1.0);
	}
	else if	(p < 1.0)
	{
		r = sqrt(-log(1. - p));
		val = (((2.32121276858 * r + 4.85014127135) * r
			- 2.29796479134) * r - 2.78718931138)
			/ ((1.63706781897 * r + 3.54388924762) * r + 1.0);
	}
	else
	{
		return ExceptionValue(1.);
	}

	val = val - (pnormal(val, 0.0, 1.0) - p) / dnormal(val, 0.0, 1.0);
	return mu + sigma * val;
}
