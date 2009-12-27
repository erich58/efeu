/*
latin-9 Ausgabekonvertierung

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

#define	U1(c)	((c & 0x80) == 0)	/* Test auf 1-Byte Sequenz */
#define	U2(c)	((c & 0xe0) == 0xc0)	/* Test auf 2-Byte Sequenz */
#define U3(c)	((c & 0xf0) == 0xe0)	/* Test auf 3-Byte Sequenz */
#define	U4(c)	((c & 0xf8) == 0xf0)	/* Test auf 4-Byte Sequenz */

#define	UF(c)	((c & 0xc0) == 0x80)	/* Test auf Folgezeichen */

static void set_buf (IO *out, int c, int n)
{
	int i;

	for (i = 1; i < IO_MAX_SAVE; i++)
		out->save_buf[i] = (i < n);

	out->save_buf[0] = c;
	out->nsave = 1;
}

static void latin9_flush (IO *out)
{
	int i;

	if	(!out->nsave)	return;

	if	(out->save_buf[out->nsave]) /* Unvollständig */
	{
		for (i = 0; i < out->nsave; i++)
			io_putc(out->save_buf[i], out->data);
	}
	else
	{
		char *p = (char *) out->save_buf;
		int32_t c = pgetucs(&p, out->nsave);
		io_putc(ucs_to_latin9(c), out->data);
	}

	out->nsave = 0;
}

static int latin9_putucs (int32_t c, IO *io)
{
	latin9_flush(io);
	io_putc(ucs_to_latin9(c), io->data);
	return 1;
}

static int latin9_put (int c, IO *io)
{
	if	(c == EOF)
	{
		latin9_flush(io);
		return c;
	}

	if	(UF(c) && io->nsave && io->save_buf[io->nsave])
	{
		io->save_buf[io->nsave++] = c;
		return c;
	}

	latin9_flush(io);

	if	(U2(c))	set_buf(io, c, 2);
	else if	(U3(c))	set_buf(io, c, 3);
	else if	(U4(c))	set_buf(io, c, 4);
	else		io_putc(c, io->data);

	return c;
}

static int latin9_ctrl (IO *io, int req, va_list list)
{
	switch (req)
	{
	case IO_FLUSH:

		latin9_flush(io);
		return io_vctrl(io->data, req, list);

	case IO_CLOSE:

		latin9_flush(io);
		return io_close(io->data);

	case IO_IDENT:

		*va_arg(list, char **) = io_xident(io->data, "latin9(%*)");
		return 0;

	default:

		return io_vctrl(io->data, req, list);
	}
}


IO *out_latin9 (IO *base)
{
	if	(base)
	{
		IO *io = io_alloc();
		io->putucs = latin9_putucs;
		io->put = latin9_put;
		io->ctrl = latin9_ctrl;
		io->data = base;
		return io;
	}

	return NULL;
}
