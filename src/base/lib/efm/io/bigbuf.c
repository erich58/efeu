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
#include <EFEU/Debug.h>
#include <fcntl.h>

#define FMT_31	"[ftools:31]$!: could not create tempoaray file $1.\n"
#define FMT_32	"[ftools:32]$!: could not truncate file $1.\n"
#define FMT_33	"[ftools:33]$!: output error in file $1.\n"
#define FMT_34	"[ftools:34]$!: input error in file $1.\n"
#define FMT_35	"[ftools:35]$!: seeking in file $1 failed.\n"

#define	DEF_SIZE	0xfff0	/* 65 kB - 16 */

#define	TMP_PFX		"Buf"
#define	TMP_DIR		NULL

int ftruncate (int fd, off_t length);

typedef struct {
	char *name;	/* Name der temporären Zwischendatei */
	int fd;		/* Filedeskriptor */
	unsigned char *buf;	/* Datenbuffer */
	unsigned size;	/* Buffergröße */
	unsigned pos;	/* Aktuelle Position */
	unsigned end;	/* Bufferende */
	unsigned blk_pos;	/* Aktueller Block */
	unsigned blk_end;	/* Zahl der Blöcke */
} BIGBUF;

static void bb_debug (BIGBUF *bb, const char *cmd)
{
	dbg_message("bigbuf", DBG_TRACE, "$1($2)\n", NULL, "ss", cmd, bb->name);
}

static void bb_error (BIGBUF *bb, const char *fmt)
{
	dbg_error(NULL, fmt, "s", bb->name);
}

static void bb_flush (BIGBUF *bb)
{
	if	(bb->fd == EOF)
	{
		if	(bb->name == NULL)
			bb->name = newtemp(TMP_DIR, TMP_PFX);

		bb->fd = open(bb->name, O_RDWR|O_CREAT|O_TRUNC, 0600);

		if	(bb->fd == EOF)
			bb_error(bb, FMT_31);

		bb_debug(bb, "open");
	}
	else if	(bb->blk_pos < bb->blk_end)
	{
		if	(ftruncate(bb->fd, bb->blk_pos * bb->size) < 0)
			bb_error(bb, FMT_32);
	}

	if	(write(bb->fd, bb->buf, bb->pos) != bb->pos)
		bb_error(bb, FMT_33);

	bb->blk_pos++;
	bb->blk_end = bb->blk_pos;
	bb->end = bb->pos = 0;
}

static int bb_ctrl (void *ptr, int req, va_list list)
{
	BIGBUF *bb = ptr;

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
				bb_error(bb, FMT_35);
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
			deltemp(bb->name);

		lfree(bb->buf);
		memfree(bb);
		return 0;

	default:

		return EOF;
	}
}

static int bb_put (int c, void *ptr)
{
	BIGBUF *bb = ptr;

	if	(bb->pos >= bb->size)
		bb_flush(bb);

	bb->buf[bb->pos++] = c;
	return c;
}

static int bb_get (void *ptr)
{
	BIGBUF *bb = ptr;

	while (bb->pos >= bb->end)
	{
		int nread;
		bb->pos = 0;

		if	(bb->blk_pos >= bb->blk_end)
			return EOF;

		nread = read(bb->fd, bb->buf, bb->size);

		if	(nread < 0)
			bb_error(bb, FMT_34);

		bb->end = nread;
		bb->blk_pos++;
	}

	return bb->buf[bb->pos++];
}


IO *io_bigbuf (size_t size, const char *pfx)
{
	IO *io;
	BIGBUF *bb;

	bb = memalloc(sizeof(BIGBUF));

	if	(pfx)
	{
		char *dir = mdirname(pfx, 0);
		char *name = mbasename(pfx, NULL);
		bb->name = newtemp(dir, name);
		memfree(dir);
		memfree(name);
	}

	bb->fd = EOF;
	bb->size = size ? size : DEF_SIZE;
	bb->buf = lmalloc(bb->size);

	io = io_alloc();
	io->data = bb;
	io->get = bb_get;
	io->put = bb_put;
	io->ctrl = bb_ctrl;
	return io;
}
