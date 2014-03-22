/*
String/Zeichen mit Sonderdarstellung ausgeben

$Copyright (C) 1993, 2008 Erich Frühstück
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


#include <EFEU/io.h>
#include <EFEU/patcmp.h>
#include <ctype.h>

#define	EXTEND	1	/* Zeichen 128 - 255 unverändert ausgeben */
#define	LATIN	0	/* Zeichen 161 - 255 unverändert ausgeben */

#if	EXTEND
#define	printable(c)	isprint(c) || ((unsigned char) (c) > 127)
#elif	LATIN
#define	printable(c)	isprint(c) || ((unsigned char) (c) > 160)
#else
#define	printable(c)	isprint(c)
#endif

/*
String mit Sonderdarstellung ausgeben
*/

int sb_xputs (const char *str, StrBuf *sb, const char *delim)
{
	if	(sb && str)
	{
		const unsigned char *p = (const unsigned char *) str;
		int n;

		for (n = 0; *p; p++)
			n += sb_xputc(*p, sb, delim);

		return n;
	}

	return 0;
}


/*
Zeichen mit Sonderdarstellung ausgeben
*/

int sb_xputc (int c, StrBuf *sb, const char *delim)
{
	int flag;

	if	(sb == NULL || c == EOF)
		return 0;

	flag = 1;
	c = (unsigned char) c;

	switch (c)
	{
	case '\b':	c = 'b'; break;
	case '\f':	c = 'f'; break;
	case '\n':	c = 'n'; break;
	case '\r':	c = 'r'; break;
	case '\t':	c = 't'; break;
	case '\v':	c = 'v'; break;
	case '\\':	break;
#if	ESCEXT
	case 033:	c = 'e'; break;
	case 127:	c = 'd'; break;
#endif
	default:	flag = listcmp(delim, c); break;
	}

	if	(flag)
	{
		sb_putc('\\', sb);
		sb_putc(c, sb);
		return 2;
	}

	if	(printable(c))
	{
		sb_putc(c, sb);
		return 1;
	}

	sb_putc('\\', sb);
	sb_putc('0' + (c >> 6) % 8, sb);
	sb_putc('0' + (c >> 3) % 8, sb);
	sb_putc('0' + (c % 8), sb);
	return 4;
}
