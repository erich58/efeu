/*
IO-Schnittstelle zu großen Zwischenbuffer

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
#include <EFEU/ftools.h>
#include <EFEU/procenv.h>
#include <fcntl.h>

#define	DEF_SIZE	0x10000	/* 65 kB */

#define	TMP_PFX		"Buf"
#define	TMP_DIR		NULL

extern char *tempnam (const char *, const char *);

typedef struct {
	char *name;	/* Name der temporären Zwischendatei */
	int fd;		/* Filedeskriptor */
	uchar_t *buf;	/* Datenbuffer */
	unsigned size;	/* Buffergröße */
	unsigned pos;	/* Aktuelle Position */
	unsigned end;	/* Bufferende */
	unsigned blk_pos;	/* Aktueller Block */
	unsigned blk_end;	/* Zahl der Blöcke */
} BIGBUF;

int bigbuf_debug = 0;

static void bb_debug (BIGBUF *bb, const char *cmd)
{
	if	(bigbuf_debug)
		io_printf(ioerr, "bigbuf: %s(%s)\n", cmd,  bb->name);
}

static void bb_error (BIGBUF *bb, int err)
{
	message("bigbuf", MSG_FTOOLS, err, 1, bb->name);
	exit(EXIT_FAILURE);
}

static void bb_flush (BIGBUF *bb)
{
	if	(bb->fd == EOF)
	{
		if	(bb->name == NULL)
			bb->name = tempnam(TMP_DIR, TMP_PFX);

		bb->fd = open(bb->name, O_RDWR|O_CREAT|O_TRUNC, 0600);

		if	(bb->fd == EOF)
			bb_error(bb, 31);

		bb_debug(bb, "open");
	}
	else if	(bb->blk_pos < bb->blk_end)
	{
		if	(ftruncate(bb->fd, bb->blk_pos * bb->size) < 0)
			bb_error(bb, 32);
	}

	if	(write(bb->fd, bb->buf, bb->pos) != bb->pos)
		bb_error(bb, 33);

	bb->blk_pos++;
	bb->blk_end = bb->blk_pos;
	bb->end = bb->pos = 0;
}

static int bb_ctrl (BIGBUF *bb, int req, va_list list)
{
	switch (req)
	{
	case IO_IDENT:

		*va_arg(list, char **) = msprintf("bigbuf(%s) %d+%d %d+%d",
			bb->name, bb->blk_pos, bb->pos, bb->blk_end, bb->end);
		return 0;

	case IO_REWIND:

		if	(bb->blk_pos)
		{
			bb_flush(bb);

			if	(lseek(bb->fd, 0, SEEK_SET) < 0)
				bb_error(bb, 35);
		}

		bb->blk_end = bb->blk_pos;
		bb->end = bb->pos;

		bb->pos = 0;
		bb->blk_pos = 0;
		return 0;

	case IO_CLOSE:

		if	(bb->fd != EOF)
		{
			bb_debug(bb, "close");
			close(bb->fd);
		}

		if	(bb->name)
		{
			remove(bb->name);
			free(bb->name);
		}

		lfree(bb->buf);
		memfree(bb);
		return 0;

	default:

		return EOF;
	}
}

static int bb_put (int c, BIGBUF *bb)
{
	if	(bb->pos >= bb->size)
		bb_flush(bb);

	bb->buf[bb->pos++] = c;
	return c;
}

static int bb_get (BIGBUF *bb)
{
	while (bb->pos >= bb->end)
	{
		bb->pos = 0;

		if	(bb->blk_pos >= bb->blk_end)
			return EOF;

		bb->end = read(bb->fd, bb->buf, bb->size);

		if	(bb->end < 0)
			bb_error(bb, 34);

		bb->blk_pos++;
	}

	return bb->buf[bb->pos++];
}


io_t *io_bigbuf(size_t size, const char *pfx)
{
	io_t *io;
	BIGBUF *bb;
	fname_t *fname;

	bb = memalloc(sizeof(BIGBUF));

	if	((fname = strtofn(pfx)) != NULL)
	{
		bb->name = tempnam(fname->path, fname->name);
		memfree(fname);
	}

	bb->fd = EOF;
	bb->size = size ? size : DEF_SIZE;
	bb->buf = lmalloc(bb->size);

	io = io_alloc();
	io->data = bb;
	io->get = (io_get_t) bb_get;
	io->put = (io_put_t) bb_put;
	io->ctrl = (io_ctrl_t) bb_ctrl;
	return io;
}
