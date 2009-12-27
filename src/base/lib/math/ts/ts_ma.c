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

#define	SetData(func, data, a, b, c)	\
	CallFunc(&Type_double, (data) + (a), func, (data) + (b), (data) + (c))

TimeSeries *ts_ma (TimeSeries *ts, int n, int adjust)
{
	TimeSeries *base;
	double gew;
	int i, j;

	if	(ts == NULL)	return NULL;
	if	(n <= 0)	return NULL;

	if	(n > ts->dim)	n = ts->dim;

	gew = 1. / (double) n;
	base = ts_create(ts->name, ts->base, n);
	ts->base.value += (n - 1);
	ts->dim -= (n - 1);

	for (i = 0; i < n; i++)
		base->data[i] = ts->data[i];

	for (i = 0; i < ts->dim; i++)
	{
		double x = ts->data[i];

		for (j = 1; j < n; j++)
			x += ts->data[i + j];

		ts->data[i] = gew * x;
	}

	return base;
}


void ts_ima (TimeSeries *ts, TimeSeries *base, int adjust)
{
	double gew;
	int i;
	
	if	(ts->base.type != base->base.type)
	{
		dbg_note(NULL, "[TimeSeries:1]",
			"cc", ts->base.type, base->base.type);
		return;
	}

	gew = (double) base->dim;
	ts_sync(ts, ts->base, ts->dim + base->dim - 1, 0);
	ts->base.value = base->base.value;

	for (i = ts->dim; i-- > base->dim;)
	{
		double *x = ts->data + i - base->dim;
		ts->data[i] = gew * (x[1] - x[0]);
	}

	for (i = 0; i < base->dim; i++)
		ts->data[i] = base->data[i];

	for (i = base->dim; i < ts->dim; i++)
		ts->data[i] += ts->data[i - base->dim];
}
