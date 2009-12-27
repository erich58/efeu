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


static int cr_get (IO *io);
static int cr_ctrl (IO *io, int req, va_list list);

IO *io_crfilter (IO *io)
{
	IO *new;

	new = io_alloc();
	new->get = cr_get;
	new->ctrl = cr_ctrl;
	new->data = io;
	return new;
}


/*	Zeichen lesen
*/

static int cr_get (IO *io)
{
	int c;

	do	c = io_getc(io->data);
	while	(c == '\r' || c == 32);

	return c;
}


/*	Kontrollfunktion
*/

static int cr_ctrl (IO *io, int req, va_list list)
{
	switch (req)
	{
	case IO_CLOSE:	return io_close(io->data);
	case IO_REWIND:	return io_rewind(io->data);
	default:	return io_vctrl(io->data, req, list);
	}
}
