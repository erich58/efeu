/*
:*:io interface to string data pool
:de:IO-Zugriff auf Sammelbecken für Zeichenketten

$Header	<EFEU/$1>

$Copyright (C) 2007 Erich Frühstück
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

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <errno.h>
#include <EFEU/ioctrl.h>
#include <EFEU/StrPool.h>
#include <EFEU/mstring.h>
#include <EFEU/Debug.h>
#include <EFEU/parsub.h>

static int pool_put (int c, IO *io)
{
	StrPool *pool = io->data;

	if	(pool->used + 1 >= pool->msize)
		StrPool_expand(pool, 1);

	return (unsigned char) (pool->mdata[pool->used++] = (char) c);
}

static int pool_ctrl (IO *io, int req, va_list list)
{
	StrPool *pool = io->data;

	switch (req)
	{
	case IO_CLOSE:
		pool_put(0, io);
		rd_deref(pool);
		return 0;
	case IO_IDENT:
		*va_arg(list, char **) = "<StrPool>";
		return 0;
	default:
		return EOF;
	}
}


IO *StrPool_open (StrPool *pool)
{
	if	(pool)
	{
		IO *io = io_alloc();
		io->put = pool_put;
		io->data = rd_refer(pool);
		io->ctrl = pool_ctrl;
		return io;
	}
	else	return NULL;
}

size_t StrPool_vprintf (StrPool *pool, const char *fmt, va_list args)
{
	size_t pos;
	IO *out;

	pos = StrPool_offset(pool);
	out = StrPool_open(pool);
	io_vxprintf(out, fmt, args);
	io_close(out);
	return pos;
}

size_t StrPool_printf (StrPool *pool, const char *fmt, ...)
{
	va_list args;
	size_t pos;
	IO *out;

	pos = StrPool_offset(pool);
	out = StrPool_open(pool);
	va_start(args, fmt);
	io_vxprintf(out, fmt, args);
	va_end(args);
	io_close(out);
	return pos;
}

size_t StrPool_psubarg (StrPool *pool, const char *fmt, const char *def, ...)
{
	va_list args;
	size_t pos;
	IO *out;

	pos = StrPool_offset(pool);
	out = StrPool_open(pool);
	va_start(args, def);
	io_psubvarg(out, fmt, def, args);
	va_end(args);
	io_close(out);
	return pos;
}
