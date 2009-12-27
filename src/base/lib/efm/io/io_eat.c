/*
Zeichen überlesen

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
#include <EFEU/iocpy.h>
#include <EFEU/patcmp.h>


static void c_skip (IO *io);

int io_eat(IO *io, const char *wmark)
{
	int c;

	while ((c = io_mgetc(io, 1)) != EOF)
	{
		if	(c == '/' && iocpy_cskip(io, NULL, c,
				NULL, 1) != EOF)
		{
			continue;
		}
		else if	(!listcmp(wmark, c))
		{
			io_ungetc(c, io);
			break;
		}
	}

	return c;
}


/*	Lesen bis Ende eines C - Style Kommentars
*/

static void c_skip(IO *io)
{
	int c;

	while ((c = io_getc(io)) != EOF)
	{
		while (c == '*')
		{
			c = io_getc(io);

			if	(c == '/')
			{
				return;
			}
		}
	}
}


/*	C, C++ Kommentare überlesen
*/

int iocpy_cskip(IO *in, IO *out, int c, const char *arg, unsigned int flags)
{
	int x;

	x = io_getc(in);

	if	(x == '*')
	{
		io_protect(in, 1);
		c_skip(in);
		io_protect(in, 0);
		return 0;
	}
	else if	(x == '/' && flags)
	{
		return iocpy_skip(in, out, x, "\n", 1);
	}
	else
	{
		io_ungetc(x, in);
		return EOF;
	}
}


/*	Zeichen überlesen
*/

int iocpy_skip(IO *in, IO *out, int c, const char *arg, unsigned int flags)
{
	int x;

	while ((x = io_getc(in)) != EOF)
		if (listcmp(arg, x)) break;

/*	Bei Flag wird Abschlusszeichen zurückgeschrieben
*/
	if	(flags)	io_ungetc(x, in);

	return 0;
}
