/*
Argument lesen

$Copyright (C) 2006 Erich Frühstück
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
#include <EFEU/strbuf.h>

static void sub_copy (IO *io, StrBuf *buf, int delim, int flag)
{
	int c;

	while ((c = io_getc(io)) != EOF)
	{
		if	(c == delim)
		{
			if	(flag)
				sb_putc(c, buf);

			return;
		}

		sb_putc(c, buf);

		switch (c)
		{
		case '[':	sub_copy(io, buf, ']', 1); break;
		case '(':	sub_copy(io, buf, ')', 1); break;
		case '{':	sub_copy(io, buf, '}', 1); break;
		}
	}
}

char *io_getarg (IO *io, int beg, int end)
{
	int c = beg ? io_getc(io) : 0;

	if	(c == beg)
	{
		StrBuf *buf = sb_acquire();
		sub_copy(io, buf, end, 0);
		return sb_cpyrelease(buf);
	}

	io_ungetc(c, io);
	return NULL;
}
