/*	Filterskript

$Copyright (C) 2004 Erich Frühstück
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

#include <EFEU/EDB.h>
#include <EFEU/preproc.h>

static void subcopy (StrBuf *buf, IO *io, int delim)
{
	int c;

	while ((c = io_getc(io)) != EOF)
	{
		sb_putc(c, buf);

		if	(c == delim)	return;

		switch (c)
		{
		case '{':	subcopy(buf, io, '}'); break;
		case '[':	subcopy(buf, io, ']'); break;
		case '(':	subcopy(buf, io, ')'); break;
		}
	}
}

static void maincopy (StrBuf *buf, IO *io)
{
	int c;
	int space;

	space = 0;

	while ((c = io_mgetc(io, 1)) != EOF)
	{
		if	(c == '\n')
		{
			int d = io_peek(io);
			space = 0;

			if	(d == ' ' || d == '\t')
			{
				do	c = io_getc(io);
				while	(c == ' ' || c == '\t');

				io_ungetc(c, io);
				continue;
			}
		}
		else if	(c == '\t' || c == ' ')
		{
			space = ' ';
			continue;
		}

		if	(space)
		{
			sb_putc(space, buf);
			space = 0;
		}

		sb_putc(c, buf);

		switch (c)
		{
		case '{':	subcopy(buf, io, '}'); break;
		case '[':	subcopy(buf, io, ']'); break;
		case '(':	subcopy(buf, io, ')'); break;
		}
	}

	sb_putc(0, buf);
}

EDB *edb_script (EDB *edb, IO *io)
{
	if	(edb && io)
	{
		StrBuf *buf = sb_create(0);
		io = io_cmdpreproc(io);
		maincopy(buf, io);
		edb = edb_filter(edb, buf->data);
		sb_destroy(buf);
	}
	
	io_close(io);
	return edb;
}

static EDB *fdef_script (EDBFilter *filter, EDB *base,
	const char *opt, const char *arg)
{
	return arg ? edb_script(base, io_fileopen(arg, "r")) : base;
}

EDBFilter EDBFilter_script = {
	"script", "=path", fdef_script, NULL,
	":*:load filter from script"
	":de:Filter aus Datei laden"
};
