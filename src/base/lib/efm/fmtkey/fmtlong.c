/*
Ganzzahlwert formatieren

$Copyright (C) 1994 Erich Fr�hst�ck
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
#include <EFEU/locale.h>

#define	LOCALE(name)	(Locale.print ? Locale.print->name : NULL)


static char *lc_digit = "0123456789abcdef";
static char *uc_digit = "0123456789ABCDEF";

int fmt_long(io_t *io, const fmtkey_t *key, long xval)
{
	strbuf_t *sb;
	ulong_t val;
	int n, k, sig, base;
	int ptrval;
	char *p, *sep, *digit, *mark;

/*	Konvertierungsparameter bestimmen
*/
	sig = 0;
	base = 10;
	digit = lc_digit;
	sep = NULL;
	mark = NULL;
	ptrval = 0;

	switch (key->mode)
	{
	case 'i':	sep = LOCALE(thousands_sep); /* FALLTHROUGH */
	case 'd':	sig = 1; break;
	case 'b':	base = 2; break;
	case 'o':	base = 8; break;
	case 'P':	ptrval = 1; /* FALLTHROUGH */
	case 'X':	digit = uc_digit; mark = "0X"; base = 16; break;
	case 'p':	ptrval = 1; /* FALLTHROUGH */
	case 'x':	mark = "0x"; base = 16; break;
	default:	break;
	}

	if	(!(ptrval || (key->flags & FMT_ALTMODE)))
		mark = NULL;

/*	Datenwert setzen
*/
	if	(ptrval || key->flags & FMT_LONG)
	{
		if	(sig && xval < 0)
		{
			val = -xval;
			sig = -1;
		}
		else	val = xval;
	}
	else if	(key->flags & FMT_SHORT)
	{
		short x = xval;

		if	(!sig)
		{
			val = (unsigned short) x;
		}
		else if	(x < 0)
		{
			val = -x;
			sig = -1;
		}
		else	val = x;
	}
	else
	{
		int x = xval;

		if	(!sig)
		{
			val = (unsigned int) x;
		}
		else if	(x < 0)
		{
			val = -x;
			sig = -1;
		}
		else	val = x;
	}

/*	Ziffern zwischenspeichern (Verkehrte Reihenfolge !)
*/
	sb = new_strbuf(32);
	p = NULL;

	for (n = 0; val != 0; n++)
	{
		if	(sep && n % 3 == 0)
			sb_rputs(p, sb), p = sep;

		sb_putc(digit[val % base], sb);
		val /= base;
	}

	if	(key->mode == 'o' && (key->flags & FMT_ALTMODE))
	{
		if	(sep && n % 3 == 0)
			sb_rputs(p, sb), p = sep;

		sb_putc(digit[0], sb);
		n++;
	}

	if	(n == 0 && (key->flags & FMT_NOPREC))
	{
		sb_putc(digit[0], sb);
		n++;
	}

	while (sb->pos < key->prec)
	{
		if	(sep && n % 3 == 0)
			sb_rputs(p, sb), p = sep;

		sb_putc(digit[0], sb);
		n++;
	}

/*	Auff�llen mit Nullen
*/
	if	(key->flags & FMT_ZEROPAD && key->width > sb->pos)
	{
		k = key->width;

		if	(mark)	k -= strlen(mark);

		if	(sig == -1 || (key->flags & FMT_BLANK))	k--;

		while (sb->pos < k)
		{
			if	(sep && n % 3 == 0)
			{
				if	(p == NULL)
				{
					p = sep;
				}
				else if	(sb->pos + strlen(p) < k)
				{
					sb_rputs(p, sb);
				}
				else	break;
			}

			sb_putc(digit[0], sb);
			n++;
		}
	}

/*	Kennung und Vorzeichen ausgeben
*/
	sb_rputs(mark, sb);
	ftool_addsig (sb, sig, key->flags);
	return ftool_ioalign (io, sb, key);
}
