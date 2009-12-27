/*
:*:print time series data
:de:Zeitreihendaten ausgeben

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

int ts_print(IO *io, const TimeSeries *ts, const char *fmt)
{
	int i, n;

	if	(ts == NULL)	return 0;

	n = io_printf(io, "TimeSeries(%#s, \"", ts->name);
	n += tindex_print(io, ts->base, 0);
	n += io_puts("\"", io);

	if	(ts->fmt)	fmt = ts->fmt;
	if	(!fmt)		fmt = "%#10.2f";

	if	(ts->dim > 0)
	{
		for (i = 0; i < ts->dim; i++)
		{
			if	(i % 6 == 0)	n += io_puts(",\n", io);
			else			n += io_puts(", ", io);

			n += io_printf(io, fmt, ts->data[i]);
		}

		n += io_puts("\n", io);
	}

	n += io_puts(")\n", io);
	return n;
}
