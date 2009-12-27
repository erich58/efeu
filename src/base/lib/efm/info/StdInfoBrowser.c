/*
Standard Info-Browser

$Copyright (C) 2006 Erich Frühstück
This file is part of EFEU.

EFEU is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public
License as published by the Free Software Foundation; either
version 2 of the License, or (at your option) any later version.

EFEU is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty
of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU General Public License for more details.

You should have received a copy of the GNU General Public
License along with EFEU; see the file COPYING.
If not, write to the Free Software Foundation, Inc.,
59 Temple Place, Suite 330, Boston, MA 02111-1307, USA.
*/

#include <EFEU/procenv.h>
#include <EFEU/Info.h>
#include <EFEU/Debug.h>

static int Lines = 24;

static char *getline (IO *io, StrBuf *buf)
{
	int c;

	sb_clean(buf);

	while ((c = io_getc(io)) != EOF)
	{
		if	(c == '\n')
		{
			sb_putc(0, buf);
			return (char *) buf->data;
		}

		sb_putc(c, buf);
	}

	return NULL;
}

void StdInfoBrowser (const char *name)
{
	InfoNode *info, *base;
	StrBuf *buf;
	IO *io;
	char *p;

	base = GetInfo(NULL, name);
	info = rd_refer(base);

	if	((p = getenv("LINES")))
		Lines = atoi(p);

	io = io_interact (" ", NULL);
	buf = sb_create(0);

	for (;;)
	{
		if	(info)
		{
			IO *out = io_fileopen(Pager, "w");
			PrintInfo(out, info);
			io_close(out);
			rd_deref(base);
			base = rd_refer(info);
		}

		io_putc('[', io);

		if	(base && base->prev)
			InfoName(io, NULL, base);
		else	io_putc('/', io);

		io_putc(']', io);

		if	(!(p = getline(io, buf)))
			break;
		
		rd_deref(info);
		info = GetInfo(base, p);
	}

	sb_destroy(buf);
	io_close(io);
}
