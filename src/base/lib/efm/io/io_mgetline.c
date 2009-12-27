/*
Zeile mit Buffer einlesen

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
#include <EFEU/ioctrl.h>
#include <EFEU/patcmp.h>

char *io_mgetline (IO *io, StrBuf *buf, const char *delim)
{
	static STRBUF(static_buf, 0);
	int c;

	if	(io_peek(io) == EOF)	return NULL;

	if	(!buf)
		buf = &static_buf;

	sb_begin(buf);

	while ((c = io_mgetc(io, 0)) != EOF)
	{
		if	(c == '\\')
		{
			c = io_mgetc(io, 0);

			if	(c == EOF)
			{
				c = '\\';
			}
			else if	(!listcmp(delim, c))
			{
				sb_putc('\\', buf);
			}
		}
		else if	(listcmp(delim, c))
		{
			io_ungetc(c, io);
			break;
		}

		sb_putc(c, buf);
	}

	sb_putc(0, buf);

	if	(buf == &static_buf)
		return sb_memcpy(buf);

	return (char *) buf->data;
}
