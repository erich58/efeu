/*
:*:binary read/write of time series
:de:Binäres lesen/schreiben von Zeitreihen

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

TimeSeries *ts_read (IO *io)
{
	if	(io_peek(io) != EOF)
	{
		TimeIndex base;
		TimeSeries *ts;

		base.type = io_getval(io, 1);
		base.value = io_getval(io, 3);
		ts = ts_create(NULL, base, io_getval(io, 4));
		ReadData(&Type_str, &ts->name, io);
		ReadVecData(&Type_double, ts->dim, ts->data, io);
		return ts;
	}
	else	return NULL;
}

int ts_write (IO *io, const TimeSeries *ts)
{
	if	(ts && io)
	{
		io_putval(ts->base.type, io, 1);
		io_putval(ts->base.value, io, 3);
		io_putval(ts->dim, io, 4);
		WriteData(&Type_str, &ts->name, io);
		WriteVecData(&Type_double, ts->dim, ts->data, io);
		return 1;
	}
	else	return 0;
}
