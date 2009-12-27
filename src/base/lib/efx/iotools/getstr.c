/*
Strings laden

$Copyright (C) 1995 Erich Frühstück
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

#include <EFEU/efio.h>

static char *substring(IO *io, int delim)
{
	char *p;
	char buf[2];

	buf[0] = delim;
	buf[1] = 0;
	io_getc(io);
	p = io_xgets(io, buf);
	io_getc(io);
	return p;
}

static char *loadstring(IO *io, int delim)
{
	int no_nl;
	int at_begin;
	StrBuf *buf;
	int c;

	io_getc(io);
	c = io_mgetc(io, 1);

	if	(c == delim)	return NULL;

	io_protect(io, 1);
	buf = sb_create(0);
	at_begin = 1;
	no_nl = 1;

	if	(c == '\n')
	{
		no_nl = 0;
		c = io_mgetc(io, 1);
	}

	while (c != EOF)
	{
		if	(at_begin)
		{
			if	(c == '\\')
			{
				c = io_mgetc(io, 1);

				if	(c != delim)
					sb_putc('\\', buf); 
			}
			else if	(c == delim)
			{
				break;
			}
		}

		sb_putc(c, buf);
		at_begin = (no_nl || c == '\n');
		c = io_mgetc(io, 1);
	}

	io_protect(io, 0);
	return sb2str(buf);
}


char *getstring(IO *io)
{
	int c;

	switch (c = io_eat(io, " \t"))
	{
	case '\n':	io_getc(io);
	case EOF:	return NULL;
	case '(':	return loadstring(io, ')');
	case '{':	return loadstring(io, '}');
	case '[':	return loadstring(io, ']');
	case '<':	return loadstring(io, '>');
	case '`':	return loadstring(io, '\'');
	case '\'':
	case '"':	return substring(io, c);
	case '/':
	case '!':
	case '|':	return loadstring(io, c);
	default:	return io_mgets(io, "\n");
	}
}
