/*
:*:	divide output structure
:de:	Ausgabestruktur aufteilen

$Copyright (C) 2003 Erich Frühstück
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

typedef struct {
	IO *base;	/* Ausgabestruktur */
	StrBuf buf;	/* Zwischebuffer */
} DIVIDE;

static int divide_put (int c, IO *io)
{
	DIVIDE *div = io->data;
	return sb_putc(c, &div->buf);
}

static int divide_ctrl (IO *io, int req, va_list list)
{
	DIVIDE *div = io->data;

	switch (req)
	{
	case IO_CLOSE:
		io_write(div->base, div->buf.data, div->buf.pos);
		sb_free(&div->buf);
		return io_close(div->base);
	case IO_FLUSH:
		io_write(div->base, div->buf.data, div->buf.pos);
		/* FALLTHROUGH */
	case IO_REWIND:
		sb_clean(&div->buf);
		return 0;
	case IO_IDENT:
		*va_arg(list, char **) = io_xident(div->base, "divide(%*)");
		return 0;
	default:
		break;
	}

	return EOF;
}

/*
:*:
The function |$1| creates a output structure <out> associated with <base>.
If <out> is closed, all data written to <out> are copied to <base>.
The argument <bsize> defines the block size of the temporary data buffer.
If 0, a default block sizte is used.
:de:
Die Funktion |$1| generiert eine mit <base> verbundene Ausgabestruktur <out>.
Falls <out> geschlossen wird, werden alle nach <out> geschriebenen Daten nach
<base> kopiert. Das Argument <bsize> definiert die Blockgröße des temporären
Datenbuffers. Falls es null ist, wird eine vordefinierte Buffergröße verwendet.
*/

IO *io_divide (IO *base, size_t bsize)
{
	if	(base)
	{
		DIVIDE *div = memalloc(sizeof *div);
		div->base = io_refer(base);
		sb_init(&div->buf, bsize);
		base = io_alloc();
		base->put = divide_put;
		base->ctrl = divide_ctrl;
		base->data = div;
	}

	return base;
}
