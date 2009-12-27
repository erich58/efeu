/*
String mit Markierung ausgeben

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

int io_mputc(int c, IO *io, const char *delim)
{
	int n = 0;

	if	(listcmp(delim, c))
		if	(io_putc('\\', io) != EOF)	n++;

	if	(io_putc(c, io) != EOF)	n++;

	return n;
}


int io_mputs(const char *str, IO *io, const char *delim)
{
	if	(str != NULL)
	{
		const unsigned char *p = (const unsigned char *) str;
		int n;

		for (n = 0; *p != 0; p++)
			n += io_mputc(*p, io, delim);

		return n;
	}
	else	return 0;
}
