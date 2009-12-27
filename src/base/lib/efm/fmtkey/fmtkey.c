/*
Formatparameter abfragen

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

#include <EFEU/fmtkey.h>
#include <EFEU/strbuf.h>
#include <ctype.h>

int io_fmtkey(IO *io, FmtKey *key)
{
	FmtKey buf;
	int c;
	int n;

	if	(key == NULL)	key = &buf;

	key->mode = 0;
	key->flags = FMT_RIGHT;
	key->width = 0;
	key->prec = 0;
	key->list = NULL;
	c = io_getc(io);
	n = 1;

/*	Steuerflags
*/
	for (;;)
	{
		if	(c == ' ')	key->flags |= FMT_BLANK;
		else if	(c == '+')	key->flags |= FMT_SIGN|FMT_BLANK;
		else if	(c == '-')	key->flags &= ~FMT_RIGHT;
		else if	(c == '0')	key->flags |= FMT_ZEROPAD;
		else if	(c == '#')	key->flags |= FMT_ALTMODE;
		else			break;

		c = io_getc(io);
		n++;
	}

/*	Feldbreite
*/
	while (isdigit(c))
	{
		key->width *= 10;
		key->width += c - '0';
		c = io_getc(io);
		n++;
	}

	if	(c == '*')
	{
		key->flags |= FMT_NEED_WIDTH;
		c = io_getc(io);
		n++;
	}

/*	Genauigkeit
*/
	if	(c == '.')
	{
		c = io_getc(io);
		n++;

		if	(c == '-')
		{
			key->flags |= FMT_NEGPREC;
			c = io_getc(io);
			n++;
		}

		while (isdigit(c))
		{
			key->prec *= 10;
			key->prec += c - '0';
			c = io_getc(io);
			n++;
		}

		if	(c == '*')
		{
			key->flags |= FMT_NEED_PREC;
			c = io_getc(io);
			n++;
		}
	}
	else	key->flags |= FMT_NOPREC;

/*	Variablentype
*/
	for (;;)
	{
		if	(c == 'h')
		{
			key->flags |= (key->flags & FMT_SHORT) ?
				FMT_BYTE : FMT_SHORT;
		}
		else if	(c == 'l')
		{
			key->flags |= (key->flags & FMT_LONG) ?
				FMT_XLONG : FMT_LONG;
		}
		else if	(c == 'L')
		{
			key->flags |= FMT_XLONG;
		}
		else	break;

		c = io_getc(io);
		n++;
	}

	if	(c == EOF)
		return n - 1;

	key->mode = c;

	if	(key->mode == '[')
	{
		StrBuf *sb;
		int escape;

		sb = sb_create(0);
		escape = 0;

		while ((c = io_getc(io)) != EOF)
		{
			n++;

			if	(c == '\\')
			{
				c = io_getc(io);
				n++;

				if	(c != ']')
					sb_putc('\\', sb);
			}
			else if	(c == ']')
			{
				break;
			}

			sb_putc(c, sb);
		}

		key->list = sb2str(sb);
	}

	return n;
}

int fmtkey(const char *fmt, FmtKey *key)
{
	IO *io = io_cstr(fmt);
	int n = io_fmtkey(io, key);
	io_close(io);
	return n;
}
