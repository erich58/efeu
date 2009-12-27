/*
Zeichen/String ausgeben

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


/*	Zeichen Ausgeben
*/

int io_putc(int c, io_t *io)
{
	return (io && io->put) ? io->put(c, io->data) : 0;
}


/*	Zeichen wiederholt ausgeben
*/

int io_nputc (int c, io_t *io, int n)
{
	if	(n > 0 && io && io->put)
	{
		register int i;

		for (i = 0; i < n; i++)
			if (io->put(c, io->data) == EOF) break;

		return i;
	}
	else	return 0;
}


/*	String ausgeben
*/

int io_puts (const char *str, io_t *io)
{
	if	(io && io->put && str)
	{
		register const uchar_t *p;
		register int n;

		p = (const uchar_t *) str;

		for (n = 0; *p != 0; p++, n++)
			if (io->put(*p, io->data) == EOF) break;

		return n;
	}
	else	return 0;
}

/*	String mit Zeilenvorschub ausgeben
*/

int io_nlputs(const char *str, io_t *io)
{
	if	(io && io->put && str)
	{
		register const uchar_t *p;
		register int n, last;

		p = (const uchar_t *) str;
		last = '\n';

		for (n = 0; *p != 0; last = *(p++), n++)
			if (io->put(*p, io->data) == EOF) break;

		if (last != '\n')
			if (io->put('\n', io->data) != EOF) n++;

		return n;
	}
	else	return 0;
}

/*	IO-Struktur umkopieren
*/

int io_copy (io_t *in, io_t *out)
{
	int c, n;

	for (n = 0; (c = io_getc(in)) != EOF; n++)
		io_putc(c, out);

	return n;
}
