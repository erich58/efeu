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

static FKEY_DECL(buf);

FmtKey *fmtkey (FmtKey *key, int32_t (*get) (void *data), void *data)
{
	int32_t c;

	if	(!key)	key = &buf;

	key->mode = 0;
	key->flags = FMT_RIGHT;
	key->size = 0;
	key->width = 0;
	key->prec = 0;
	sb_trunc(&key->buf);
	c = get(data);

/*	Steuerflags
*/
	for (;;)
	{
		if	(c == ' ')	key->flags |= FMT_BLANK;
		else if	(c == '+')	key->flags |= FMT_SIGN|FMT_BLANK;
		else if	(c == '-')	key->flags &= ~FMT_RIGHT;
		else if	(c == '0')	key->flags |= FMT_ZEROPAD;
		else if	(c == '#')	key->flags |= FMT_ALTMODE;
		else if	(c == '\'')	key->flags |= FMT_GROUP;
		else			break;

		c = get(data);
	}

/*	Feldbreite
*/
	while (isdigit(c))
	{
		key->width *= 10;
		key->width += c - '0';
		c = get(data);
	}

	if	(c == '*')
	{
		key->flags |= FMT_NEED_WIDTH;
		c = get(data);
	}

/*	Genauigkeit
*/
	if	(c == '.')
	{
		c = get(data);

		if	(c == '-')
		{
			key->flags |= FMT_NEGPREC;
			c = get(data);
		}

		while (isdigit(c))
		{
			key->prec *= 10;
			key->prec += c - '0';
			c = get(data);
		}

		if	(c == '*')
		{
			key->flags |= FMT_NEED_PREC;
			c = get(data);
		}
	}
	else	key->flags |= FMT_NOPREC;

/*	Variablentype
*/
	for (;;)
	{
		if	(c == 'h')
			key->size = (key->size == FMTKEY_SHORT) ?
				FMTKEY_BYTE : FMTKEY_SHORT;
		else if (c == 'l')
			key->size = (key->size == FMTKEY_LONG) ?
				FMTKEY_XLONG : FMTKEY_LONG;
		else if (c == 'L')
			key->size = FMTKEY_XLONG;
		else if (c == 'z')
			key->size = FMTKEY_SIZE;
		else if (c == 'j')
			key->size = FMTKEY_IMAX;
		else if (c == 't')
			key->size = FMTKEY_PDIFF;
		else	break;

		c = get(data);
	}

	key->mode = c;

	if	(key->mode == '[')
	{
		StrBuf *sb;
		int escape;

		sb = &key->buf;
		escape = 0;

		while ((c = get(data)) != EOF)
		{
			if	(c == '\\')
			{
				c = get(data);

				if	(c != ']')
					sb_putc('\\', sb);
			}
			else if	(c == ']')
			{
				break;
			}

			sb_putc(c, sb);
		}

		sb_nul(sb);
	}

	return key;
}

int32_t fmtkey_pgetc (void *data)
{
	char **ptr = data;
	return (**ptr) ? *(*ptr)++ : EOF;
}

int32_t fmtkey_pgetucs (void *data)
{
	return pgetucs(data, 4);
}

