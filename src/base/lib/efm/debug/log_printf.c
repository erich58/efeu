/*
Datenwerte formatiern

$Copyright (C) 2009 Erich Frühstück
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

#include <EFEU/io.h>

void log_printf (LogControl *ctrl, const char *fmt, ...)
{
	IO *io = LogOpen(ctrl);

	if	(io)
	{
		va_list args;

		va_start(args, fmt);
		io_vxprintf(io, fmt, args);
		va_end(args);
		io_close(io);
	}
}


void log_vprintf (LogControl *ctrl, const char *fmt, va_list list)
{
	IO *io = LogOpen(ctrl);

	if	(io)
	{
		io_vxprintf(io, fmt, list);
		io_close(io);
	}
}
