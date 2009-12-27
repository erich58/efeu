/*
IO-Struktur mit Aufräumfunktion

$Copyright (C) 1999 Erich Frühstück
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

#include <EFEU/ioctrl.h>


/*	Eingabestruktur
*/

typedef struct {
	void (*clean) (io_t *io, void *par);
	io_t *io;	/* Eingabestruktur */
	void *par;	/* Aufräumfunktion */
} CLPAR;


static int clpar_get (CLPAR *clpar)
{
	return io_getc(clpar->io);
}

static int clpar_put (int c, CLPAR *clpar)
{
	return io_putc(c, clpar->io);
}


static int clpar_ctrl (CLPAR *clpar, int req, va_list list)
{
	if	(req == IO_CLOSE)
	{
		int stat;

		clpar->clean(clpar->io, clpar->par);
		stat = io_close(clpar->io);
		memfree(clpar);
		return stat;
	}

	return io_vctrl (clpar->io, req, list);
}


io_t *io_cleanup (io_t *io, void (*cf) (io_t *io, void *p), void *p)
{
	if	(io && cf)
	{
		CLPAR *clpar = memalloc(sizeof(CLPAR));
		clpar->clean = cf;
		clpar->io = io;
		clpar->par = p;
		io = io_alloc();
		io->get = (io_get_t) clpar_get;
		io->put = (io_put_t) clpar_put;
		io->ctrl = (io_ctrl_t) clpar_ctrl;
		io->data = clpar;
	}

	return io;
}
