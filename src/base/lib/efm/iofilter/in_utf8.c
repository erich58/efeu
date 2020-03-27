/*
UTF-8 Eingabekonvertierung

$Copyright (C) 2009 Erich Frühstück
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

static int utf8_get (IO *io)
{
	int32_t c = io_getucs(io->data);

	if	(c < 0)
	{
		return EOF;
	}
	else if	(c <= 0x0000007F)
	{
		return c;
	}
	else if	(c <= 0x000007FF)
	{
		io->nsave = 1;
		io->save_buf[0] = 0x80 | (c & 0x3f);
		return 0xc0 | ((c >> 6) & 0x1f);
	}
	else if	(c <= 0x0000FFFF)
	{
		io->nsave = 2;
		io->save_buf[0] = 0x80 | (c & 0x3f);
		io->save_buf[1] = 0x80 | ((c >> 6) & 0x3f);
		return 0xe0 | ((c >> 12) & 0x0f);
	}
	else if	(c <= 0x001FFFFF)
	{
		io->nsave = 3;
		io->save_buf[0] = 0x80 | (c & 0x3f);
		io->save_buf[1] = 0x80 | ((c >> 6) & 0x3f);
		io->save_buf[2] = 0x80 | ((c >> 12) & 0x3f);
		return 0xf0 | ((c >> 18) & 0x07);
	}
	else	return 0;
}

static int utf8_ctrl (IO *io, int req, va_list list)
{
	switch (req)
	{
	case IO_IDENT:

		*va_arg(list, char **) = io_xident(io->data, "in_utf8(%*)");
		return 0;

	default:

		return io_vctrl(io->data, req, list);
	}
}


IO *in_utf8 (IO *base)
{
	if	(base)
	{
		IO *io = io_alloc();
		io->get = utf8_get;
		io->ctrl = utf8_ctrl;
		io->data = base;
		return io;
	}

	return NULL;
}
