/*
Binäre Ein- Ausgabe von vorzeichenlosen Ganzzahlwerten

$Copyright (C) 2006 Erich Frühstück
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


uint64_t io_ullget (IO *io, size_t recl)
{
	uint64_t uval;
	int c;
	int i;

	if	(recl > 8)	recl = 8;

	if	((c = io_getc(io)) == EOF)
	{
		io_error(io, "[ftools:21]", NULL);
		return 0;
	}

	uval = c;

	for (i = 1; i < recl; i++)
	{
		if	((c = io_getc(io)) == EOF)
		{
			io_error(io, "[ftools:21]", NULL);
			break;
		}

		uval = (uval << 8) + c;
	}

	return uval;
}

void io_ullput (uint64_t val, IO *io, size_t recl)
{
	unsigned char buf[8];
	int i;

	if	(recl > 8)	recl = 8;

	for (i = 0; i < recl; i++)
	{
		buf[i] = (val & 0xff);
		val >>= 8;
	}

	for (i = recl - 1; i >= 0; i--)
		if (io_putc(buf[i], io) == EOF)
			io_error(io, "[ftools:22]", NULL);
}
