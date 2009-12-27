/*
String/Zeichen mit Sonderdarstellung ausgeben

$Copyright (C) 1993 Erich Frühstück
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

#define	ESCEXT	0	/* \e=^[ und \d=^? - Erweiterung verwenden */
#define	LATIN1	1	/* Zeichen 161 - 255 unverändert ausgeben */

#if	LATIN1
#define	printable(c)	isprint(c) || ((unsigned char) (c) > 160)
#else
#define	printable(c)	isprint(c)
#endif


/*	String von Sonderdarstellung ausgeben
*/

int io_xputs(const char *str, IO *io, const char *delim)
{
	if	(str != NULL)
	{
		const unsigned char *p = (const unsigned char *) str;
		int n;

		for (n = 0; *p != 0; p++)
			n += io_xputc(*p, io, delim);

		return n;
	}

	return 0;
}


/*	Zeichen mit Sonderdarstellung ausgeben
*/

int io_xputc(int c, IO *io, const char *delim)
{
	int pos;
	int flag;
	char buf[8];

	pos = 0;
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
		buf[pos++] = '\\';
		buf[pos++] = (char) c;
	}
	else if	(printable(c))
	{
		buf[pos++] = (char) c;
	}
	else
	{
		return io_printf(io, "\\%03o", c);
	}
	
	buf[pos] = 0;
	return io_puts(buf, io);
}
