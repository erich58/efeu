/*
Binäre Ein- Ausgabe von Ganzzahlwerten

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


int64_t io_llget (IO *io, size_t recl)
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

	if	(c & 0x80)
	{
		uval = 0;
		
		for (i = recl; i < 8; i++)
			uval = (uval << 8) + 0xff;

		uval = (uval << 8) + c;
	}
	else	uval = c;

	for (i = 1; i < recl; i++)
	{
		if	((c = io_getc(io)) == EOF)
		{
			io_error(io, "[ftools:21]", NULL);
			break;
		}

		uval = (uval << 8) + c;
	}

	return (int64_t) uval;
}

void io_llput (int64_t val, IO *io, size_t recl)
{
	int sig;
	int i;
	uint64_t uval;
	unsigned char buf[8];

	if	(recl > 8)	recl = 8;

	sig = (val < 0) ? 0x80 : 0;
	uval = val;

	for (i = 0; i < recl; i++)
	{
		buf[i] = (uval & 0xff);
		uval >>= 8;
	}

	recl--;
	buf[recl] = (buf[recl] & 0x7f) | sig;

	for (i = recl; i >= 0; i--)
		if (io_putc(buf[i], io) == EOF)
			io_error(io, "[ftools:22]", NULL);
}
