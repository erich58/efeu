/*
Zeichen parsen

$Copyright (C) 2022 Erich Frühstück
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

#include <EFEU/ioscan.h>

const char *io_scan_char(IO *io, StrBuf *sb)
{
	int c = io_peek(io);

	if	(c != '\'')
		return NULL;

	if	(!sb)
	{
		static STRBUF(buf, 32);
		sb = &buf;
		sb_clean(sb);
	}

	io_protect(io, 1);
	io_getc(io);
	c = io_getc(io);

	if	(c == '\\')
	{
		int d = io_peek(io);

		if	(d > 0x7f)
		{
			c = io_getucs(io);
		}
		else if	(d == '\'')
		{
			c = io_getc(io);
		}
		else if	(d > 0)
		{
			io_ungetc(c, io);
			c = io_xgetc(io, NULL);
		}
	}
	else if	(c == '\'')
	{
		io_ungetc(c, io);
		c = EOF;
	}
	else if	(c > 0x7f)
	{
		io_ungetc(c, io);
		c = io_getucs(io);
	}

	sb_putucs(c, sb);

	while ((c = io_getucs(io)) != EOF)
	{
		if	(c == '\'')
			break;

		io_xprintf(ioerr, "extra character %lc "
			"in character constant ignored.\n", c);
	}

	if	(io_peek(io) == '\'')
		io_getc(io);

	io_protect(io, 0);
	sb_putc(0, sb);
	return (const char *) sb->data;
}
