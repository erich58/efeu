/*
Integer parsen

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
#include <stdint.h>

#define	OCT	"01234567"
#define	DEC	"01234567890"
#define	HEX	"01234567890ABCDEFabcdef"

#if	IO_SCAN_BINARY
static void cpy_binary(IO *io, StrBuf *sb)
{
	uintmax_t val = 0;
	int c = io_getc(io);

	while (c != EOF)
	{
		if	(c == '0')	val = 2 * val;
		else if	(c == '1')	val = 2 * val + 1;
		else			break;

		c = io_getc(io);
	}

	io_ungetc(c, io);

	char buf[64];
	sb_putc('x', sb);
	snprintf(buf, sizeof buf, "%jx", val);
	sb_puts(buf, sb);
}
#endif

static void cpy_digit(IO *io, StrBuf *sb, const char *digits)
{
	int c = io_getc(io);

	while (strchr(digits, c))
	{
		sb_putc(c, sb);
		c = io_getc(io);
	}

	io_ungetc(c, io);
}

const char *io_scan_int(IO *io, StrBuf *sb)
{
	int c = io_getc(io);
	int sig = 0;

	if	(c == '+' || c == '-')
	{
		sig = c;
		c = io_getc(io);
	}

	if	(!strchr(DEC, c))
	{
		io_ungetc(c, io);

		if	(sig)
			io_ungetc(sig, io);

		return NULL;
	}

	if	(!sb)
	{
		static STRBUF(buf, 32);
		sb = &buf;
		sb_clean(sb);
	}

	if	(sig)
		sb_putc(sig, sb);

	sb_putc(c, sb);
	io_protect(io, 1);

	if	(c == '0')
	{
		c = io_getc(io);

		if	(c == 'x' || c == 'X')
		{
			sb_putc(c, sb);
			cpy_digit(io, sb, HEX);
		}
#if	IO_SCAN_BINARY
		else if	(c == 'b' || c == 'B')
		{
			cpy_binary(io, sb);
		}
#endif
		else
		{
			io_ungetc(c, io);
			cpy_digit(io, sb, OCT);
		}
	}
	else
	{
		cpy_digit(io, sb, DEC);
	}

	io_protect(io, 0);
	return sb_cstr(sb);
}
