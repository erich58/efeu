/*
:*:moving average
:de:Gleitende Mittelwerte

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

void ts_ma (TimeSeries *ts, int n)
{
	double gew;
	int i, j;

	if	(ts == NULL || ts->dim <= 1 || n <= 1)
		return;

	if	(n > ts->dim)	n = ts->dim;

	gew = 1. / (double) n;
	ts->base.value += (n - 1);
	ts->dim -= (n - 1);

	for (i = 0; i < ts->dim; i++)
	{
		double x = ts->data[i];

		for (j = 1; j < n; j++)
			x += ts->data[i + j];

		ts->data[i] = gew * x;
	}
}

void ts_cma (TimeSeries *ts, int n)
{
	double g1, g2;
	int i, j;
	
	if	(ts == NULL || ts->dim <= 1 || n <= 1)
		return;

	if	(ts->dim < n)
	{
		for (i = 1; i < ts->dim; i++)
			ts->data[0] += ts->data[i];

		ts->data[0] /= (double) ts->dim;
		ts->base.value += ts->dim / 2;
		ts->dim = 1;
		return;
	}

	if	(n % 2 == 0)
	{
		n = n / 2;
		g2 = 0.5 / (double) n;
		g1 = 0.5 * g2;
	}
	else
	{
		n = (n - 1) / 2;
		g1 = g2 = 1. / (2. * n + 1.);
	}

	ts->dim -= 2 * n;
	ts->base.value += n;

	for (i = 0; i < ts->dim; i++)
	{
		ts->data[i] += ts->data[i + 2 * n];
		ts->data[i] *= g1;

		for (j = 1; j < n; j++)
			ts->data[i] += g2 * (ts->data[i + j] + ts->data[i + j + n]);
		ts->data[i] += g2 * ts->data[i + n];
	}
}
