/*
Bereich einlesen

$Copyright (C) 1999 Erich Frühstück
This file is part of EFEU.

EFEU is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public
License as published by the Free Software Foundation; either
version 2 of the License, or (at your option) any later version.

EFEU is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty
of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU General Public License for more details.

You should have received a copy of the GNU General Public
License along with EFEU; see the file COPYING.
If not, write to the Free Software Foundation, Inc.,
59 Temple Place, Suite 330, Boston, MA 02111-1307, USA.
*/

#include <efeudoc.h>
#include <ctype.h>

char *DocParseRegion (IO *in, const char *delim)
{
	StrBuf *buf;
	int last;
	int c;

	buf = new_strbuf(0);
	last = 0;

	while ((c = io_getc(in)) != EOF)
	{
		if	(delim && c == '\n')
		{
			sb_putc(0, buf);

			if	(patcmp(delim, (char *) buf->data + last, NULL))
			{
				buf->pos = last;
				sb_sync(buf);
				break;
			}

			buf->data[buf->pos - 1] = '\n';
			last = buf->pos;
		}
		else	sb_putc(c, buf);
	}

	return sb2str(buf);
}
