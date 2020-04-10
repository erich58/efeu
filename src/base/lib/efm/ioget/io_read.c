/*
Lesen von Ein/Ausgabestruktur

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


size_t io_read(IO *io, void *buf, size_t nbyte)
{
	unsigned char *p;
	size_t n;
	int c;

	p = (unsigned char *) buf;

	for (n = 0; n < nbyte && (c = io_getc(io)) != EOF; n++)
		p[n] = (unsigned char) c;

	return n;
}


size_t io_rread(IO *io, void *buf, size_t nbyte)
{
	unsigned char *p;
	size_t n;
	int c;

	p = (unsigned char *) buf;

	for (n = 1; n <= nbyte && (c = io_getc(io)) != EOF; n++)
		p[nbyte - n] = (unsigned char) c;

	return n - 1;
}


/*	Daten in dynamischen Buffer lesen
*/

char *io_mread(IO *io, size_t size)
{
	char *buf;

	buf = memalloc(size + 1);
	size = io_read(io, buf, size);
	buf[size] = 0;
	return buf;
}
