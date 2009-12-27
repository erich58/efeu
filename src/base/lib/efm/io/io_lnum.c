/*
IO-Struktur mit Zeilenzähler

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
#include <EFEU/ioctrl.h>


/*	Eingabestruktur
*/

typedef struct {
	IO *io;		/* Eingabestruktur */
	unsigned nl : 1;	/* Flag für neue Zeile */
	unsigned line : 31;	/* Zeilennummer */
} LNUM;


/*	Zeichen lesen
*/

static int lnum_get (IO *io)
{
	LNUM *lnum = io->data;
	int c;

	lnum->line += lnum->nl;
	c = io_getc(lnum->io);
	lnum->nl = (c == '\n' ? 1 : 0);
	return c;
}


/*	Kontrollfunktion
*/

static int lnum_ctrl (IO *io, int req, va_list list)
{
	LNUM *lnum = io->data;
	int stat;

	switch (req)
	{
	case IO_UNGETC:

		lnum->nl = 0;
		return io_ungetc(va_arg(list, int), lnum->io);

	case IO_CLOSE:

		stat = io_close(lnum->io);
		memfree(lnum);
		return stat;

	case IO_REWIND:

		if	(io_rewind(lnum->io) == EOF)	return EOF;

		lnum->nl = 0;
		lnum->line = 1;
		return 0;
		
	case IO_IDENT:

		*va_arg(list, char **) = io_xident(lnum->io,
			"%*:%d", lnum->line);
		return 0;

	case IO_LINE:

		return lnum->line;

	default:

		return io_vctrl(lnum->io, req, list);
	}
}


IO *io_lnum (IO *io)
{
	if	(io && io_ctrl(io, IO_LINE) == EOF)
	{
		LNUM *lnum = memalloc(sizeof(LNUM));
		lnum->io = io;
		lnum->nl = 0;
		lnum->line = 1;
		io = io_alloc();
		io->get = lnum_get;
		io->ctrl = lnum_ctrl;
		io->data = lnum;
	}

	return io;
}
