/*
Zahl der ausgegebenen Zeichen zählen

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


typedef struct {
	io_t *io;	/* Ausgabestruktur */
	size_t count;	/* Zahl der ausgegebenen Zeichen */
} COUNT;

static int std_put(int c, COUNT *count)
{
	if	((c = io_putc(c, count->io)) != EOF)
		count->count++;

	return c;
}

static int dummy_put(int c, COUNT *count)
{
	count->count++;
	return c;
}

static int count_ctrl(COUNT *count, int req, va_list list)
{
	size_t stat;

	switch (req)
	{
	case IO_CLOSE:

		io_close(count->io);
		stat = count->count;
		memfree(count);
		break;

	case IO_REWIND:

		if	(io_rewind(count->io) == EOF)
			return EOF;

		stat = count->count;
		count->count = 0;
		break;

	default:

		stat = io_vctrl(count->io, req, list);
		break;
	}

	return stat;
}


io_t *io_count (io_t *io)
{
	COUNT *count = memalloc(sizeof(COUNT));
	count->io = io;
	count->count = 0;
	io = io_alloc();
	io->put = (io_put_t) (io ? std_put : dummy_put);
	io->ctrl = (io_ctrl_t) count_ctrl;
	io->data = count;
	return io;
}
