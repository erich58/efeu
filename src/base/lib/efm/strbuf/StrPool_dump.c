/*
:*:write out contents of string pool
:de:Inhalt eines Stringbuffers ausgeben

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
#include <EFEU/mstring.h>
#include <EFEU/Debug.h>
#include <EFEU/parsub.h>

static void do_dump (IO *io, char *data, size_t size, size_t offset)
{
	size_t n;
	int start;

	for (start = 1, n = 0; n < size; n++)
	{
		if	(start)
		{
			io_printf(io, "%7zu \"", n + offset);
			start = 0;
		}

		if	(data[n] == 0)
		{
			io_putc('"', io);
			io_putc('\n', io);
			start = 1;
		}
		else	io_xputc(data[n], io, "\"");
	}
}

void StrPool_dump (StrPool *pool, IO *out)
{
	if	(pool && out)
	{
		io_puts("  ----- ------------\n", out);
		do_dump(out, pool->cdata, pool->csize, 0);
		io_puts("  ----- ------------\n", out);
		do_dump(out, pool->mdata, pool->used, pool->csize);
		io_puts("  ----- ------------\n", out);
	}
}
