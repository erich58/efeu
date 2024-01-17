/*
:*:input format conversions
:de:Eingabeabfrage

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


#include <EFEU/ioscan.h>
#include <EFEU/mstring.h>
#include <EFEU/unicode.h>
#include <ctype.h>

static int make_char (IO *io, int type, void **ptr);
static int is_ucs(IO *io);

int io_scan (IO *io, unsigned int flags, void **ptr)
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
			StrBuf *sb = sb_acquire();

			do	io_scan_str(io, sb);
			while	((c = io_eat(io, " \t")) == '"');

			p = mstrcpy(sb_cstr(sb));
			sb_release(sb);
			type = SCAN_STR;
		}
	}
	else if	(c == '\'')
	{
		if	(flags & SCAN_XCHAR)
		{
			return make_char(io, flags, ptr);
		}
	}
	else if	(c == 'L' && (flags & SCAN_UCS) && is_ucs(io))
	{
		return make_char(io, SCAN_UCS, ptr);
	}
	else if	(c == 'N' && (flags & (SCAN_NAME|SCAN_STR))
			&& io_testkey(io, "NULL"))
	{
		type = (flags & SCAN_NAME) ? SCAN_NAME : SCAN_STR;
	}
	else if	(isalpha(c) || c == '_')
	{
		if	(flags & SCAN_NAME)
		{
			p = io_getname(io);
			type = SCAN_NAME;
		}
	}
	else if	(flags & SCAN_NUMERIC)
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

char *io_getname(IO *io)
{
	int c = io_eat(io, " \t");

	if	(isalpha(c) || c == '_')
	{
		StrBuf *sb = sb_acquire();
		c = io_getc(io);

		while (isalnum(c) || c == '_')
		{
			sb_putc(c, sb);
			c = io_mgetc(io, 1);
		}

		io_ungetc(c, io);
		return sb_cpyrelease(sb);
	}

	return NULL;
}

static int is_ucs(IO *io)
{
	int c = io_getc(io);

	if	(io_peek(io) == '\'')
		return 1;

	io_ungetc(c, io);
	return 0;
}

static int make_char (IO *io, int type, void **ptr)
{
	static int32_t buf_ucs;
	static unsigned char buf_char;
	int32_t val;

	const char *p = io_scan_char(io, NULL);
	val = p ? pgetucs(&p, 4) : 0;

	type &= (SCAN_UCS | SCAN_CHAR);

	if	(type == (SCAN_UCS | SCAN_CHAR))
		type = (val > 0x7f) ? SCAN_UCS : SCAN_CHAR;

	if	(ptr == NULL)
	{
		;
	}
	else if	(type & SCAN_UCS)
	{
		buf_ucs = val;
		*ptr = &buf_ucs;
	}
	else if	(type & SCAN_CHAR)
	{
		buf_char = val;
		*ptr = &buf_char;
	}

	return type;
}
