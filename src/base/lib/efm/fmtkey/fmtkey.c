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


int fmtkey(const char *fmt, fmtkey_t *key)
{
	fmtkey_t buf;
	int n;

	if	(key == NULL)	key = &buf;

	key->mode = 0;
	key->flags = FMT_RIGHT;
	key->width = 0;
	key->prec = 0;
	key->list = NULL;
	n = 0;

/*	Steuerflags
*/
	for (;;)
	{
		if	(fmt[n] == ' ')	key->flags |= FMT_BLANK;
		else if	(fmt[n] == '+')	key->flags |= FMT_SIGN|FMT_BLANK;
		else if	(fmt[n] == '-')	key->flags &= ~FMT_RIGHT;
		else if	(fmt[n] == '0')	key->flags |= FMT_ZEROPAD;
		else if	(fmt[n] == '#')	key->flags |= FMT_ALTMODE;
		else			break;

		n++;
	}

/*	Feldbreite
*/
	while (isdigit(fmt[n]))
	{
		key->width *= 10;
		key->width += fmt[n++] - '0';
	}

	if	(fmt[n] == '*')
	{
		key->flags |= FMT_NEED_WIDTH;
		n++;
	}

/*	Genauigkeit
*/
	if	(fmt[n] == '.')
	{
		n++;

		if	(fmt[n] == '-')
		{
			key->flags |= FMT_NEGPREC;
			n++;
		}

		while (isdigit(fmt[n]))
		{
			key->prec *= 10;
			key->prec += fmt[n++] - '0';
		}

		if	(fmt[n] == '*')
		{
			key->flags |= FMT_NEED_PREC;
			n++;
		}
	}
	else	key->flags |= FMT_NOPREC;

/*	Variablentype
*/
	for (;;)
	{
		if	(fmt[n] == 'h')	key->flags |= FMT_SHORT;
		else if	(fmt[n] == 'l')	key->flags |= FMT_LONG;
		else if	(fmt[n] == 'L')	key->flags |= FMT_XLONG;
		else			break;

		n++;
	}

	key->mode = fmt[n];

	if	(key->mode != 0)	n++;

	if	(key->mode == '[')
	{
		strbuf_t *sb;
		int escape;

		sb = new_strbuf(0);
		escape = 0;

		while (fmt[n] != 0)
		{
			if	(fmt[n] == '\\')
			{
				if	(fmt[n + 1] == ']')	n++;
			}
			else if	(fmt[n] == ']')
			{
				n++;
				break;
			}
			else if	(fmt[n] == 0)		break;

			sb_putc(fmt[n++], sb);
		}

		key->list = sb2str(sb);
	}

	return n;
}
