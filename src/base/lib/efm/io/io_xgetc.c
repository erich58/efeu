/*
Zeichen mit Sonderbedeutung lesen

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

#include <EFEU/io.h>
#include <EFEU/ioctrl.h>
#include <EFEU/patcmp.h>
#include <ctype.h>

#define	ESCEXT	0	/* \e=^[ und \d=^? - Erweiterung verwenden */

static int get_oct (IO *io, int first);
static int get_hex (IO *io);


/*	Zeichen lesen
*/

int io_xgetc (IO *io, const char *delim)
{
	int c;

	while ((c = io_getc(io)) != EOF)
	{
	/*	Escape - Sequenz
	*/
		if	(c == '\\')
		{
			switch ((c = io_getc(io)))
			{
			case EOF:	return EOF;
			case '\n':	io_linemark(io); continue;
			case '\\':	return c;
			case 'b':	return '\b';
			case 'f':	return '\f';
			case 'n':	return '\n';
			case 'r':	return '\r';
			case 't':	return '\t';
			case 'v':	return '\v';
			case 'a':	return 7;
			case '0':
			case '1':
			case '2':
			case '3':	return get_oct(io, c);
			case 'x':	return get_hex(io);
#if	ESCEXT
			case 'e':	return 27;
			case 'd':	return 127;
#endif
			default:	break;
			}

			if	(listcmp(delim, c))
				return c;

			io_ungetc(c, io);
			return '\\';
		}

	/*	Sonstige Zeichen
	*/
		else if	(listcmp(delim, c))
		{
			io_ungetc(c, io);
			break;
		}
		else	return c;
	}

	return EOF;
}


/*	Oktales Zeichen lesen
*/

static int get_oct (IO *io, int c)
{
	char buf[4];
	int i;

	buf[0] = c;

	for (i = 1; i < 3; i++)
	{
		c = io_getc(io);

		if	(c == EOF)
		{
			break;
		}
		else if	(!isdigit(c) || c == '8' || c == '9')
		{
			io_ungetc(c, io);
			break;
		}
		else	buf[i] = (char) c;
	}

	buf[i] = 0;
	return (int) strtol(buf, NULL, 8) & 0xFF;
}


/*	Hexadezimales Zeichen lesen
*/

static int get_hex (IO *io)
{
	char buf[3];
	int i, c;

	for (i = 0; i < 2; i++)
	{
		c = io_getc(io);

		if	(c == EOF)
		{
			break;
		}
		else if	(!isxdigit(c))
		{
			io_ungetc(c, io);
			break;
		}
		else	buf[i] = (char) c;
	}

	buf[i] = 0;
	return (int) strtol(buf, NULL, 16);
}
