/*
Klammerausdruck kopieren

$Copyright (C) 1994 Erich Frühstück
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

#include <EFEU/efmain.h>
#include <EFEU/efio.h>
#include <EFEU/efutil.h>


static int f_get(io_t *io)
{
	return io_egetc(io, NULL);
}

static int f_mget(io_t *io)
{
	return io_mgetc(io, 1);
}

int iocpy_brace(io_t *in, io_t *out, int c, const char *arg, unsigned int flags)
{
	int (*get) (io_t *io);
	int n;

	n = io_nputc(c, out, 1);
	get = (flags ? f_get : f_mget);

	while ((c = get(in)) != EOF)
	{
		if	(listcmp(arg, c))
		{
			n += io_nputc(c, out, 1);
			break;
		}

		switch (c)
		{
		case '(':	n += iocpy_brace(in, out, c, ")", flags); break;
		case '{':	n += iocpy_brace(in, out, c, "}", flags); break;
		case '[':	n += iocpy_brace(in, out, c, "]", flags); break;
		case '"':	n += iocpy_str(in, out, c, "\"", 1); break;
		case '\'':	n += iocpy_str(in, out, c, "'", 1); break;
		default:	n += io_nputc(c, out, 1); break;
		}
	}

	return n;
}
