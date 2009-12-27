/*
Binäre Ein- Ausgabe von Ganzzahlwerten

$Copyright (C) 1996 Erich Frühstück
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


unsigned io_getval (IO *io, int n)
{
	unsigned val;
	int c;

	for (val = 0; n > 0; n--)
	{
		if	((c = io_getc(io)) == EOF)
		{
			fprintf(stderr, "getval: Unerwartetes Fileende.\n");
			exit(EXIT_FAILURE);
		}

		val = (val << 8) + c;
	}

	return val;
}

void io_putval (unsigned val, IO *io, int n)
{
	while (n-- > 0)
	{
		if	(io_putc((val >> (8 * n)) & 0xFF, io) == EOF)
		{
			fprintf(stderr, "putval: Ausgabefehler.\n");
			exit(EXIT_FAILURE);
		}
	}
}

/*	Gepackten String einlesen
*/

char *io_getstr(IO *io)
{
	int byte;
	size_t size;

	byte = io_getc(io);

	if	(byte <= 0)	return NULL;

	size = io_getval(io, byte);
	return io_mread(io, size);
}

/*	Gepackte Ausgabe eines Strings mit Längenangabe
*/

int io_putstr(const char *str, IO *io)
{
	size_t size;
	int byte;
	int n;

	if	(str == NULL)	return io_nputc(0, io, 1);

	size = strlen(str);

	if	(size & 0xFF000000)	byte = 4;
	else if	(size & 0x00FF0000)	byte = 3;
	else if	(size & 0x0000FF00)	byte = 2;
	else				byte = 1;

	if	(io_putc(byte, io) != EOF)
	{
		n = 1 + byte;
		io_putval(size, io, byte);
		n += io_write(io, str, size);
	}
	else	n = 0;

	return n;
}
