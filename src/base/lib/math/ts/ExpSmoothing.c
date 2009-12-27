/*
:*:exponential smoothing
:de:Exponentiaesses Glätten

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

#include <Math/TimeSeries.h>

TimeSeries *ExpSmoothing (const char *name, const TimeSeries *ts,
	double alpha, double sw, int k)
{
	TimeSeries *x;
	double calpha;
	int i;

	if	(ts == NULL)	return NULL;

	if	(k < 0)	k = 0;

	x = ts_create(NULL, ts->base, ts->dim + k);

	if	(name)
		x->name = mpsubarg(name, "ns", ts->name);

	if	(alpha < 0.)	alpha = 0.;
	if	(alpha > 1.)	alpha = 1.;

	calpha = 1. - alpha;

	x->data[0] = alpha * ts->data[0] + calpha * sw;

	for (i = 1; i < ts->dim; i++)
		x->data[i] = alpha * ts->data[i] + calpha * x->data[i-1];

	for (i = ts->dim; i < x->dim; i++)
		x->data[i] = x->data[i-1];

	return x;
}

double ExpSmoothingInitial (const TimeSeries *ts, double alpha)
{
	double x, z, s1, s2, s3, calpha;
	int i;

	if	(ts == NULL)	return 0.;

	if	(alpha >= 1.)
		return ts->data[ts->dim - 1];

	if	(alpha < 0.)	alpha = 0.;

	calpha = 1. - alpha;
	s1 = 0.;
	s2 = 0.;
	s3 = 0.;
	x = 1.;
	z = 1.;

	for (i = 0; i < ts->dim; i++)
	{
		x = alpha * ts->data[i] + calpha * x;
		z = calpha * z;
		s1 += x;
		s2 += z;
		s3 += ts->data[i];
	}

	return s2 ? (s3 - s1) / s2 : ts->data[ts->dim - 1];
}

double ExpSmoothingError (const TimeSeries *ts, double alpha, double sw)
{
	double x, d, esum, calpha;
	int i;

	if	(ts == NULL)	return 0.;
	if	(alpha < 0.)	alpha = 0.;
	if	(alpha > 1.)	alpha = 1.;

	calpha = 1. - alpha;
	x = alpha * ts->data[0] + calpha * sw;
	esum = 0;

	for (i = 1; i < ts->dim; i++)
	{
		d = (ts->data[i] - x);
		esum += d * d;
		x = alpha * ts->data[i] + calpha * x;
	}

	return esum;
}
