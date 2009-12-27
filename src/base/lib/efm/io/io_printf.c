/*
Datenwerte formatiern

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

#include <EFEU/io.h>
#include <EFEU/fmtkey.h>

int io_printf(IO *io, const char *fmt, ...)
{
	if	(io && fmt)
	{
		va_list args;
		StrBuf *sb;
		int n;

		va_start(args, fmt);
		sb = sb_acquire();
		sb_vprintf(sb, fmt, args);
		n = io_puts(sb_nul(sb), io);
		sb_release(sb);
		va_end(args);
		return n;
	}

	return 0;
}


int io_vprintf(IO *io, const char *fmt, va_list list)
{
	if	(io && fmt)
	{
		StrBuf *sb;
		int n;

		sb = sb_acquire();
		sb_vprintf(sb, fmt, list);
		n = io_puts(sb_nul(sb), io);
		sb_release(sb);
		return n;
	}

	return 0;
}
