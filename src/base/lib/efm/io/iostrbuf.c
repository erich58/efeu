/*
IO Schnittstelle zu dynamischen Stringbuffer

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
#include <EFEU/strbuf.h>

static int sbuf_get (void *ptr)
{
	StrBuf *sb = ptr;
	return sb_getc(sb);
}

static int sbuf_put (int c, void *ptr)
{
	StrBuf *sb = ptr;
	return sb_putc(c, sb);
}

static int sb_nctrl (void *ptr, int req, va_list list)
{
	StrBuf *sb = ptr;

	switch (req)
	{
	case IO_REWIND:	sb->pos = 0; return 0;
	case IO_GETPOS:	*va_arg(list, unsigned *) = sb->pos; return 0;
	case IO_SETPOS:	sb->pos = *va_arg(list, unsigned *); return 0;
	case IO_CLOSE:	return 0;
	case IO_IDENT:	*va_arg(list, char **) = "<strbuf>"; return 0;
	default:	return EOF;
	}
}

static int sb_cctrl (void *ptr, int req, va_list list)
{
	StrBuf *sb = ptr;

	switch (req)
	{
	case IO_REWIND:	sb->pos = 0; return 0;
	case IO_GETPOS:	*va_arg(list, unsigned *) = sb->pos; return 0;
	case IO_SETPOS:	sb->pos = *va_arg(list, unsigned *); return 0;
	case IO_CLOSE:	sb_free(sb); memfree(sb); return 0;
	case IO_IDENT:	*va_arg(list, char **) = "<tmpbuf>"; return 0;
	default:	return EOF;
	}
}

IO *io_tmpbuf (size_t size)
{
	IO *io;
	StrBuf *sb;

	sb = memalloc(sizeof *sb);
	sb_init(sb, size);

	io = io_alloc();
	io->get = sbuf_get;
	io->put = sbuf_put;
	io->data = sb;
	io->ctrl = sb_cctrl;
	return io;
}

IO *io_strbuf (StrBuf *buf)
{
	IO *io = io_alloc();
	io->get = sbuf_get;
	io->put = sbuf_put;

	if	(!buf)
	{
		buf = memalloc(sizeof *buf);
		sb_init(buf, 0);
		io->ctrl = sb_cctrl;
	}
	else	io->ctrl = sb_nctrl;

	io->data = buf;
	return io;
}
