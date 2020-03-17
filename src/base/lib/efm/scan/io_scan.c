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
#include <ctype.h>

static int32_t get_ucs (IO *io, int delim);
static int make_char (IO *io, int type, void **ptr);

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
			return make_char(io, flags, ptr);
		}
	}
	else if	(c == 'L' && (flags & (SCAN_UTF|SCAN_UCS)))
	{
		int d;

		io_getc(io);
		d = io_peek(io);

		if	(d == '"' && (flags & SCAN_UTF))
		{
			io_getc(io);
			p = io_xgets(io, "\"");
			io_getc(io);
			type = SCAN_STR;
		}
		else if	(d == '\'' && (flags & SCAN_UCS))
		{
			return make_char(io, SCAN_UCS, ptr);
		}
		else if	(flags & SCAN_NAME)
		{
			io_ungetc(c, io);
			p = io_getname(io);
			type = SCAN_NAME;
		}
		else	io_ungetc(c, io);
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

static int32_t get_char (IO *io, int delim)
{
	int32_t c = io_getc(io);

	if	(c == delim)
		return EOF;

	if	(c == '\\')
	{
		if	(io_peek(io) == delim)
		{
			c = io_getc(io);
		}
		else
		{
			io_ungetc(c, io);
			c = io_xgetc(io, NULL);
		}
	}

	return c;
}

static int get_ucs_next (IO *io, int delim)
{
	int c = get_char(io, delim);

	if	(c == EOF)
	{
		return EOF;
	}

	if	((c & 0xc0) != 0x80)
	{
		io_ungetc(c, io);
		return EOF;
	}
	else	return (c & 0x3f); 
}

static int32_t get_ucs (IO *io, int delim)
{
	int32_t c, d;
	int n;

	c = get_char(io, delim);

	if	(c == EOF)		return EOF;
	else if	((c & 0x80) == 0)	return c;
	else if	((c & 0xc0) == 0x80)	return c;
	else if	((c & 0xe0) == 0xc0)	n = 0;
	else if	((c & 0xf0) == 0xe0)	n = 1;
	else if	((c & 0xf8) == 0xf0)	n = 2;
	/*
	else if	((c & 0xfc) == 0xf8)	n = 3;
	*/
	else				return c;

	d = get_ucs_next(io, delim);

	if	(d == EOF)
		return c;

	c &= 0x1f >> n;
	c <<= 6;
	c += d;

	for (; n; n--)
	{
		c <<= 6;
		d = get_ucs_next(io, delim);

		if	(d != EOF)
			c += d;
	}

	return c;
}

static int make_char (IO *io, int type, void **ptr)
{
	static int32_t buf_ucs;
	static unsigned char buf_char;
	int32_t val;

	io_getc(io);
	val = get_ucs(io, '\'');

	if	(val < 0)	val = 0;

	type &= (SCAN_UCS | SCAN_CHAR);

	if	(type == (SCAN_UCS | SCAN_CHAR))
		type = (val > 0xff) ? SCAN_UCS : SCAN_CHAR;

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

	while ((val = get_ucs(io, '\'')) != EOF)
	{
		io_xprintf(ioerr,
			"extra character %#lc in character constant.\n", val);
	}

	return type;
}
