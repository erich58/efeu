/*
String mit Begrenzern und ohne Transformation umkopieren

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

int iocpy_str(io_t *in, io_t *out, int c, const char *arg, unsigned int flags)
{
	int n, escape;

	if	(flags & 1)
	{
		io_putc(c, out);
		n = 1;
	}
	else	n = 0;

	escape = 0;
	io_protect(in, 1);

	while ((c = io_mgetc(in, 0)) != EOF)
	{
		if	(escape)
		{
			escape = 0;
		}
		else if	(listcmp(arg, c))
		{
			if	(flags & 2)
			{
				int d;

				d = io_mgetc(in, 0);

				if	(d == c)
				{
					io_putc(c, out);
					n++;
					continue;
				}
				else	io_ungetc(d, in);
			}

			if	(flags & 1)
			{
				io_putc(c, out);
				n++;
			}

			io_protect(in, 0);
			return n;
		}
		else	escape = (c == '\\');

		io_putc(c, out);
		n++;
	}

	io_protect(in, 0);
	return n;
}
