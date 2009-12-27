/*
:*:terminate lines with CR LF
:de:Zeilen mit CR LF abschließen

$Copyright (C) 1998 Erich Frühstück
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
#include <EFEU/mstring.h>


static int crlf_put (int c, void *ptr)
{
	if	(c == '\n')
		io_putc('\r', ptr);

	return io_putc(c, ptr);
}

static int crlf_ctrl (void *ptr, int req, va_list list)
{
	if	(req == IO_CLOSE)
		return io_close(ptr);

	return io_vctrl(ptr, req, list);
}

/*
:*:The function |$1| implements an output filter for |base| which replaces
every LF by CR and LF.
:de:Die Funktion |$1| implementiert einen Ausgabefilter für |base|, der
jedes LF durch CR LF ersetzt.
*/

IO *io_crlf (IO *base)
{
	IO *io = io_alloc();
	io->put = crlf_put;
	io->ctrl = crlf_ctrl;
	io->data = base;
	return io;
}
