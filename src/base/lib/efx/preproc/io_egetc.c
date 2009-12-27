/*
Multizeilen zusasmmenfassen

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


#include <EFEU/efmain.h>
#include <EFEU/efio.h>
#include <EFEU/efutil.h>

static int subgetc (io_t *io, const char *delim);

int io_egetc(io_t *io, const char *delim)
{
	int c;

	while ((c = subgetc(io, delim)) == '/')
	{
		if (iocpy_cskip(io, NULL, c, NULL, 1) == EOF) break;
	}

	return c;
}


static int subgetc(io_t *io, const char *delim)
{
	int c;

	c = io_getc(io);

	while (c == '\\')
	{
		c = io_getc(io);

		if	(c != EOF && c != '\n' && listcmp(delim, c))
			return c;

		io_ungetc(c, io);
		return '\\';
	}

	if	(listcmp(delim, c))
	{
		io_ungetc(c, io);
		return EOF;
	}

	return c;
}
