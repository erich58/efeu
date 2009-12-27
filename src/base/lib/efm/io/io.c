/*
Verwaltung der IO-Strukturen

$Copyright (C) 1997 Erich Frühstück
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


#include <EFEU/mstring.h>
#include <EFEU/io.h>
#include <EFEU/ioctrl.h>

static ALLOCTAB(io_tab, 0, sizeof(IO));

int io_close_stat = 0;

static void io_clean (void *ptr)
{
	IO *io = ptr;
	io_pushback(io);
	io_close_stat = io_ctrl(io, IO_CLOSE);
	del_data(&io_tab, io);
}

static char *io_ident (const void *ptr)
{
	char *name;

	if	(io_ctrl((IO *) ptr, IO_IDENT, &name) != EOF)
		return name;

	return mstrcpy("<io>");
}


IO *io_alloc (void)
{
	return rd_init(&io_reftype, new_data(&io_tab));
}


RefType io_reftype = REFTYPE_INIT("IO", io_ident, io_clean);


/*	IO-Struktur schließen/Referenzzähler verringern
*/

int io_close (IO *io)
{
	io_close_stat = 0;
	rd_deref(io);
	return io_close_stat;
}


/*	Gepufferte Zeichen zurückschreiben
*/

int io_pushback (IO *io)
{
	if	(io == NULL)	return 0;

	if	(io->nsave)
	{
		int i;

		for (i = 0; i < io->nsave; i++)
			if (io_ctrl(io, IO_UNGETC, io->save_buf[i]) == EOF)
				return EOF;

		io->nsave = 0;
	}

	if	(io->stat == IO_STAT_EOF)
		io->stat = IO_STAT_OK;

	return 0;
}


/*	Zurücksetzen der IO-Struktur
*/

int io_err (IO *io)
{
	return (io && (io->stat & IO_STAT_ERR));
}

int io_rewind (IO *io)
{
	if	(io_ctrl(io, IO_REWIND) == EOF)
		return EOF;

	io->nsave = 0;
	io->stat = 0;
	return 0;
}
