/*
Zerlegungshilfsfunktionen

$Copyright (C) 2000 Erich Frühstück
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

#include "DocBuf.h"
#include <ctype.h>


static STRBUF(parse_buf, 1024);

StrBuf *parse_open (void)
{
	sb_clear(&parse_buf);
	return &parse_buf;
}

char *parse_close (StrBuf *buf)
{
	if	(sb_getpos(buf))
	{
		sb_putc(0, buf);
		return (char *) buf->data;
	}
	else	return NULL;
}

void add_name (StrBuf *buf, IO *io, int c)
{
	while (isalnum(c) || c == '_')
	{
		sb_putc(c, buf);
		c = io_mgetc(io, 1);
	}

	io_ungetc(c, io);
}

void add_block (StrBuf *buf, IO *io, int c, int flag)
{
	IO *aus = io_strbuf(buf);
	copy_block(io, aus, c, flag);
	io_close(aus);
}

/*	Namen abfragen
*/

char *parse_name (IO *io, int c)
{
	StrBuf *buf = parse_open();
	add_name(buf, io, c);
	return parse_close(buf);
}

/*	Block abfragen
*/

char *parse_block (IO *io, int beg, int end, int flag)
{
	StrBuf *buf = parse_open();
	sb_putc(beg, buf);
	add_block(buf, io, end, flag);
	return parse_close(buf);
}


