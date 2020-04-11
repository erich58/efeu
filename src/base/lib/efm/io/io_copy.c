/*
Daten über IO-Strukturen umkopieren

$Copyright (C) 1997 Erich Frühstück
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

/*	IO-Struktur umkopieren
*/

int io_copy (IO *in, IO *out)
{
	int c, n;

	for (n = 0; (c = io_getc(in)) != EOF; n++)
		if (io_putc(c, out) == EOF) break;

	return n;
}

int io_xcopy(IO *in, IO *out, const char *delim)
{
	int n, c;

	io_protect(in, 1);
	n = 0;

	while ((c = io_xgetc(in, delim)) != EOF)
		if (io_putc(c, out) != EOF) n++;

	io_protect(in, 0);
	return n;
}

/*	Daten mit auflösen von Fortsetzunmgszeilen umkopieren
*/

int io_mcopy(IO *in, IO *out, const char *delim)
{
	int n, c;

	for (n = 0; (c = io_mgetc(in, 0)) != EOF; )
	{
		if	(c == '\\')
		{
			c = io_mgetc(in, 0);

			if	(c == EOF)
			{
				c = '\\';
			}
			else if	(!listcmp(delim, c))
			{
				io_putc('\\', out);
				n++;
			}
		}
		else if	(listcmp(delim, c))
		{
			io_ungetc(c, in);
			break;
		}

		io_putc(c, out);
		n++;
	}

	return n;
}


