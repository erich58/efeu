/*
Jahreszeitreihen Mittelwertstabil auf Monatszeitreihen umrechnen

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

#include <Math/TimeSeries.h>
#include <Math/pnom.h>

static Polynom *intpol (double *y, size_t dim)
{
	double *a;
	double *b;
	Polynom *pn;
	int i;

	pn = pnalloc(dim, 2);
	a = memalloc((dim + 1) * sizeof(double));
	b = memalloc((dim + 1) * sizeof(double));
	b[0] = 0.;

	for (i = 1; i < dim; i++)
		b[i] = (y[i] - y[i-1]) / 4.;

	b[i] = 0.;
	a[0] = 4.;

	for (i = 1; i <= dim; i++)
	{
		a[i] = 4. - 1. / a[i-1];
		b[i] -= b[i-1] / a[i-1];
	}

	b[dim] = b[dim] / a[dim];

	for (i = dim - 1; i >= 0; i--)
		b[i] = (b[i] - b[i+1]) / a[i];

	for (i = 0; i < dim; i++)
	{
		pn->x[i] = 12. * i;
		pn->c[i][0] = y[i] - 8. * b[i] - 4 * b[i+1];
		pn->c[i][1] = 2. * b[i];
		pn->c[i][2] = (b[i+1] - b[i]) / 12.;
	}

	memfree(a);
	memfree(b);
	return pn;
}

void TimeSeriesKonv_j2m (TimeSeries *ts)
{
	Polynom *pn;
	int i;

	if	(ts == NULL || ts->base.type != TS_YEAR)
	{
		dbg_note(NULL, "[TimeSeries:12]", NULL);
		return;
	}

	pn = intpol(ts->data, ts->dim);
	ts_expand(ts, ts->dim * 12);
	ts->base.type = TS_MONTH;
	ts->base.value *= 12;

	for (i = 0; i < ts->dim; i++)
		ts->data[i] = pneval(pn, 0, i + 0.5);
}
