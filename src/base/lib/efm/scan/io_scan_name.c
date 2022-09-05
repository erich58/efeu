/*
Namen parsen

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
#include <ctype.h>

const char *io_scan_name(IO *io, StrBuf *sb)
{
	int c = io_peek(io);

	if	(!(isalpha(c) || c == '_'))
		return NULL;

	if	(!sb)
	{
		static STRBUF(buf, 32);
		sb = &buf;
		sb_clean(sb);
	}

	io_protect(io, 1);
	c = io_getc(io);

	while (isalnum(c) || c == '_')
	{
		sb_putc(c, sb);
		c = io_mgetc(io, 1);
	}

	io_ungetc(c, io);
	io_protect(io, 0);
	return sb_cstr(sb);
}
