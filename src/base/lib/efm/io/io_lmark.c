/*
Ausgabezeilen markieren

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


typedef struct {
	io_t *io;	/* Ausgabestruktur */
	char *pre;	/* Zeilenanfangskennung */
	char *post;	/* Zeilenendekennung */
	int flag;	/* Flag zur Leerzeilenbehanlung */
	int newline;	/* Flag für neue Zeile */
	int lnum;	/* Zeilennummer (kann im Format verwendet werden) */
} LMARK;

static int lmark_put(int c, LMARK *lmark)
{
	if	(lmark->newline && (lmark->flag || c != '\n'))
	{
		io_printf(lmark->io, lmark->pre, lmark->lnum);
		lmark->newline = 0;
	}

	if	(c == '\n')
	{
		io_printf(lmark->io, lmark->post, lmark->lnum);
		lmark->newline = 1;
		lmark->lnum++;
	}

	return io_putc(c, lmark->io);
}

static int lmark_ctrl(LMARK *lmark, int req, va_list list)
{
	int stat;

	switch (req)
	{
	case IO_CLOSE:

		stat = io_close(lmark->io);
		memfree(lmark->pre);
		memfree(lmark->post);
		memfree(lmark);
		break;

	case IO_REWIND:

		if	(io_rewind(lmark->io) == EOF)
			return EOF;

		lmark->newline = 1;
		lmark->lnum = 1;
		stat = 0;
		break;

	case IO_IDENT:

		*va_arg(list, char **) = io_xident(lmark->io, "lmark(%*)");
		return 0;

	default:

		stat = io_vctrl(lmark->io, req, list);
		break;
	}

	return stat;
}


io_t *io_lmark (io_t *io, const char *pre, const char *post, int flag)
{
	LMARK *lmark = memalloc(sizeof(LMARK));
	lmark->io = io;
	lmark->pre = mstrcpy(pre);
	lmark->post = mstrcpy(post);
	lmark->flag = flag;
	lmark->newline = 1;
	lmark->lnum = 1;
	io = io_alloc();
	io->put = (io_put_t) lmark_put;
	io->ctrl = (io_ctrl_t) lmark_ctrl;
	io->data = lmark;
	return io;
}
