/*
Zahlenwert umkopieren

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


#include <EFEU/efmain.h>
#include <EFEU/efio.h>
#include <ctype.h>

int io_scan (io_t *io, unsigned int flags, void **ptr)
{
	int c;
	void *p;
	int type;

	c = io_eat(io, " \t");
	p = NULL;
	type = 0;

	if	(c == '"')
	{
		if	(flags & SCAN_STR)
		{
			io_getc(io);
			p = io_xgets(io, "\"");
			io_getc(io);
			type = SCAN_STR;
		}
	}
	else if	(c == '\'')
	{
		if	(flags & SCAN_CHAR)
		{
			io_getc(io);
			p = io_xgets(io, "'");
			io_getc(io);
			type = SCAN_CHAR;
		}
	}
	else if	(c == 'N' && (flags & SCAN_NULL) && io_testkey(io, "NULL"))
	{
		type = SCAN_NULL;
	}
	else if	(isalpha(c) || c == '_')
	{
		if	(flags & SCAN_NAME)
		{
			p = io_getname(io);
			type = SCAN_NAME;
		}
	}
	else if	(flags & SCAN_VALMASK)
	{
		return io_valscan(io, flags, ptr);
	}

	if	(ptr)
	{
		*ptr = p;
	}
	else	memfree(p);

	return type;
}


/*	Namen abfragen
*/

char *io_getname(io_t *io)
{
	int c = io_eat(io, " \t");

	if	(isalpha(c) || c == '_')
	{
		strbuf_t *sb = new_strbuf(0);
		c = io_getc(io);

		while (isalnum(c) || c == '_')
		{
			sb_putc(c, sb);
			c = io_mgetc(io, 1);
		}

		io_ungetc(c, io);
		return sb2str(sb);
	}

	return NULL;
}
