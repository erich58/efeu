/*
String einlesen/kopieren

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


int io_mcopy(IO *in, IO *out, const char *delim)
{
	int n, c;

	for (n = 0; (c = io_mgetc(in, 0)) != EOF; )
	{
		if	(c == '\\')
		{
			c = io_mgetc(in, 0);

			if	(c == EOF)
			{
				c = '\\';
			}
			else if	(!listcmp(delim, c))
			{
				io_putc('\\', out);
				n++;
			}
		}
		else if	(listcmp(delim, c))
		{
			io_ungetc(c, in);
			break;
		}

		io_putc(c, out);
		n++;
	}

	return n;
}


char *io_mgets(IO *io, const char *delim)
{
	StrBuf *sb;
	int c;

	sb = new_strbuf(0);

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
				sb_putc('\\', sb);
			}
		}
		else if	(listcmp(delim, c))
		{
			io_ungetc(c, io);
			break;
		}

		sb_putc(c, sb);
	}

	return sb2str(sb);
}
