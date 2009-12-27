/*
Filter für CR-LF

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
#include <EFEU/ioctrl.h>


static int cr_get (io_t *io);
static int cr_ctrl (io_t *io, int req, va_list list);

io_t *io_crfilter(io_t *io)
{
	io_t *new;

	new = io_alloc();
	new->get = (io_get_t) cr_get;
	new->ctrl = (io_ctrl_t) cr_ctrl;
	new->data = io;
	return new;
}


/*	Zeichen lesen
*/

static int cr_get(io_t *io)
{
	int c;

	do	c = io_getc(io);
	while	(c == '\r' || c == 32);

	return c;
}


/*	Kontrollfunktion
*/

static int cr_ctrl(io_t *io, int req, va_list list)
{
	switch (req)
	{
	case IO_CLOSE:	return io_close(io);
	case IO_REWIND:	return io_rewind(io);
	default:	return io_vctrl(io, req, list);
	}
}
