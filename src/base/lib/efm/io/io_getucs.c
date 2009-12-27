/*
Eingabe von UCS-Zeichen

$Copyright (C) 2007 Erich Frühstück
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

int32_t io_getucs_latin1 (IO *io)
{
	return io_getc(io);
}

int32_t io_getucs_auto (IO *io)
{
	int c, d;

	c = io_getc(io);

	if	(c < 0)		return EOF;
	if	(c < 0x80)	return c;

	if	((c & 0xc0) == 0x80 || (c & 0xf0) == 0xf0)
	{
		io->getucs = io_getucs_latin1;
		return c;
	}

	d = io_getc(io);
	io_ungetc(d, io);

	if	((d & 0xc0) != 0x80)
	{
		io->getucs = io_getucs_latin1;
		return c;
	}

	io_ungetc(c, io);
	io->getucs = io_getucs_utf8;
	return io->getucs(io);
}

int32_t io_getucs_utf8 (IO *io)
{
	int c;
	int n;
	int32_t val;

	c = io_getc(io);

	if	(c < 0)
		return EOF;

	if	((c & 0x80) == 0)
		return c;

	if	((c & 0xe0) == 0xc0)
	{
		n = 1;
		val = c & 0x1f;
	}
	else if	((c & 0xf0) == 0xe0)
	{
		n = 2;
		val = c & 0xf;
	}
	else if	((c & 0xf8) == 0xf0)
	{
		n = 3;
		val = c & 0x7;
	}
	else if	((c & 0xfc) == 0xf8)
	{
		n = 4;
		val = c & 0x3;
	}
	else if	((c & 0xfe) == 0xfc)
	{
		n = 5;
		val = c & 0x1;
	}
	else
	{
		return EOF;
	}

	do
	{
		c = io_getc(io);
		val <<= 6;

		if	(c == EOF)		;
		else if	((c & 0xc0) != 0x80)	io_ungetc(c, io);
		else				val |= (c & 0x3f);
	}
	while (--n);

	return val;
}

int32_t io_getucs (IO *io)
{
	if	(!io)	return EOF;

	if	(io->ucs_save)
	{
		io->ucs_save = 0;
		return io->ucs_char;
	}

	if	(!io->getucs)
	{
		io->getucs = io_getucs_auto;
	}

	return io->getucs(io);
}

int32_t io_ungetucs (int32_t c, IO *io)
{
	if	(io && c >= 0)
	{
		io->ucs_save = 1;
		io->ucs_char = c;
		return io->ucs_char;
	}
	else	return EOF;
}
