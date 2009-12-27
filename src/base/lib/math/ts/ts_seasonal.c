/*
:*:seasonal component of time series
:de:Saisonkomponente bestimmen

$Copyright (C) 1997 Erich Frühstück
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

#include <EFEU/object.h>
#include <Math/TimeSeries.h>

static double tsum_odd (double *x, int n)
{
	double z = x[0];

	for (; n > 0; n--)
		z += x[n] + x[-n];

	return z;
}

static double tsum_even (double *x, int n)
{
	double z = x[0] + 0.5 * (x[n] + x[-n]);

	for (n--; n > 0; n--)
		z += x[n] + x[-n];

	return z;
}


TimeSeries *ts_seasonal (const char *name, const TimeSeries *ts, int p)
{
	TimeSeries *x;
	double gew, sum;
	static double (*tsum) (double *x, int n);
	int i, j, n, N;

	if	(ts == NULL || ts->dim < 2 * p)
		return NULL;

	x = ts_create(NULL, ts->base, ts->dim);

	if	(name)
		x->name = mpsubarg(name, "ns", ts->name);

	x->fmt = mstrcpy(ts->fmt);
	n = p / 2;

	if	(ts->dim < 2 * n + p)
		return x;

	tsum = (p % 2) ? tsum_odd : tsum_even;
	gew = 1. / (double) p;
	N = ts->dim - n;

	for (i = n; i < N; i++)
		x->data[i] = ts->data[i] - gew * tsum(ts->data + i, n);

	sum = 0.;

	for (i = 0; i < p; i++)
	{
		double z = 0.;
		double y = 0.;

		for (j = n + i; j < N; j += p)
		{
			y += x->data[j];
			z += 1.;
		}

		y /= z;
		sum += y;
		x->data[(n + i) % p] = y;
	}

	sum *= gew;

	for (i = 0; i < p; i++)
		x->data[i] -= sum;

	for (j = p; j < x->dim; j++)
		x->data[j] = x->data[j - p];

	return x;
}
