/*
Printf für Stringbuffer

$Copyright (C) 1994 Erich Frühstück
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

#include <EFEU/strbuf.h>
#include <EFEU/io.h>


int sb_printf (StrBuf *sb, const char *fmt, ...)
{
	if	(sb && fmt)
	{
		va_list args;
		IO *io;
		int n;

		io = io_strbuf(sb);
		va_start(args, fmt);
		n = io_vprintf(io, fmt, args);
		va_end(args);
		io_close(io);
		return n;
	}
	else	return 0;
}

int sb_vprintf (StrBuf *sb, const char *fmt, va_list list)
{
	if	(sb && fmt)
	{
		IO *io;
		int n;

		io = io_strbuf(sb);
		n = io_vprintf(io, fmt, list);
		io_close(io);
		return n;
	}
	else	return 0;
}
