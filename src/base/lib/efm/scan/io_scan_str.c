/*
String parsen

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

const char *io_scan_str(IO *io, StrBuf *sb)
{
	if	(io_peek(io) != '"')
		return NULL;

	if	(!sb)
	{
		static STRBUF(buf, 32);
		sb = &buf;
		sb_clean(sb);
	}

	int c;
	io_protect(io, 1);
	io_getc(io);

	while ((c = io_xgetc(io, "\"")) != EOF)
		sb_putc(c, sb);

	if	(io_peek(io) == '"')
		io_getc(io);

	io_protect(io, 0);
	sb_putc(0, sb);
	return (const char *) sb->data;
}
