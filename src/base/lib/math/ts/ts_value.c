/*
:*:get value of time series
:de:Zeitreihenwert abfragen

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

double ts_value (TimeSeries *ts, TimeIndex idx, double defval)
{
	if	(ts == NULL)
		return defval;

	if	(idx.type != ts->base.type)
		idx = tindex_conv(idx, ts->base.type, 0);

	if	(idx.value <= ts->base.value)
		return ts->data[0];

	idx.value -= ts->base.value;

	if	(idx.value < ts->dim)
		return ts->data[idx.value];

	return ts->data[ts->dim - 1];
}
