/*
Zeitreihen kopieren

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


TimeSeries_t *CopyTimeSeries (const char *name, const TimeSeries_t *ts,
	int a, int b)
{
	TimeSeries_t *x;
	int i;

	if	(ts == NULL)	return NULL;
	if	(b < a)		b = a + 1;

	x = NewTimeSeries(name ? name : ts->name, ts->base, b - a);
	x->fmt = mstrcpy(ts->fmt);
	x->base.value += a;

	if	(b < 0)	return x;

	b = min(b, ts->dim);

	for (i = max(0, a); i < b; i++)
		x->data[i - a] = ts->data[i];

	return x;
}
