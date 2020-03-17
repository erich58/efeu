/*
String standardisiert ausgeben

$Copyright (C) 1994, 2008 Erich Frühstück
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

int fmt_mbstr (StrBuf *sb, const FmtKey *key, const char *str)
{
	int n, k;
	int pos;
	unsigned lim;
	int32_t c;
	char *p;

	pos = sb->pos;

	/* Nullstring behandeln */

	if	(str == NULL)
	{
		if	(key->flags & FMT_ALTMODE)
		{
			n = sb_puts("NULL", sb);
			return ftool_align(sb, key, pos, n);
		}

		return sb_nputc(' ', sb, key->width);
	}

	/* Startposition und Ausgabelimit bestimmen */

	lim = ~0;
	p = (char *) str;

	if	(key->flags & FMT_NEGPREC)
	{
		for (k = 0; pgetucs(&p, 4); k++)
			;

		p = (char *) str;

		while (k-- > key->prec)
			pgetucs(&p, 4);
	}
	else	lim = (key->flags & FMT_NOPREC) ? ~0 : key->prec;

	/* String ausgeben */

	if	(key->flags & FMT_ALTMODE)
	{
		sb_putc('"', sb);

		for (n = k = 0; k < lim && (c = pgetucs(&p, 4)); k++)
		{
			if	(c >= 0x80)
			{
				sb_putucs(c, sb);
				n++;
			}
			else	n += sb_xputc(c, sb, "'");
		}

		sb_putc('"', sb);
		n += 2;
	}
	else
	{
		for (n = 0; n < lim && (c = pgetucs(&p, 4)); n++)
			sb_putucs(c, sb);
	}

	/* Ausgabe ausrichten */

	return ftool_align(sb, key, pos, n);
}
