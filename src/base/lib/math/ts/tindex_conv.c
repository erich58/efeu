/*
:*:convert time series index
:de:Zeitreihenindex konvertieren

$Copyright (C) 1999 Erich Frühstück
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
#include <EFEU/calendar.h>
#include <Math/TimeSeries.h>

#define	MBASE(x)	((x) == TS_MONTH || (x) == TS_QUART || (x) == TS_YEAR)

#define	mittel(a,b)	(0.5 * ((a) + (b) + 1.))

static int dat2mon (unsigned dat)
{
	CalInfo *cal = Calendar(dat, NULL);
	return cal->year * 12 + cal->month - 1;
}

TimeIndex tindex_conv (TimeIndex idx, int type, int pos)
{
	if	(idx.type == type)	return idx;

	if	(MBASE(idx.type) && MBASE(type))
	{
		switch (idx.type)
		{
		case TS_YEAR:

			idx.value *= 12;

			if	(pos == 0)	idx.value += 6;
			else if	(pos > 0)	idx.value += 11;

			break;

		case TS_QUART:

			idx.value *= 3;

			if	(pos == 0)	idx.value += 1;
			else if	(pos > 0)	idx.value += 2;

			break;

		default:
			break;
		}

		switch (type)
		{
		case TS_YEAR:	idx.value /= 12; break;
		case TS_QUART:	idx.value /= 3; break;
		default:	break;
		}

		idx.type = type;
	}
	else
	{
		unsigned dat;

		if	(pos < 0)
			dat = tindex_floor(idx);
		else if	(pos > 0)
			dat = tindex_ceil(idx);
		else	dat = mittel(tindex_floor(idx), tindex_ceil(idx));

		switch (type)
		{
		case TS_YEAR:	idx.value = Calendar(dat, NULL)->year; break;
		case TS_QUART:	idx.value = dat2mon(dat) / 3; break;
		case TS_MONTH:	idx.value = dat2mon(dat); break;
		case TS_WEEK:	idx.value = (dat + 6) / 7; break;
		default:	idx.value = dat; break;
		}

		idx.type = type;
	}
	
	return idx;
}
