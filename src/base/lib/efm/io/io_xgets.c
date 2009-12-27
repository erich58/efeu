/*
String lesen/kopieren

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

#include <EFEU/io.h>
#include <EFEU/iocpy.h>


char *io_xgets(IO *io, const char *delim)
{
	int c;
	StrBuf *sb;

	sb = sb_acquire();
	io_protect(io, 1);

	while ((c = io_xgetc(io, delim)) != EOF)
		sb_putc(c, sb);

	io_protect(io, 0);
	sb_putc(0, sb);
	return sb_cpyrelease(sb);
}


int io_xcopy(IO *in, IO *out, const char *delim)
{
	int n, c;

	io_protect(in, 1);
	n = 0;

	while ((c = io_xgetc(in, delim)) != EOF)
		if (io_putc(c, out) != EOF) n++;

	io_protect(in, 0);
	return n;
}


int iocpy_xstr(IO *in, IO *out, int c, const char *arg, unsigned int flags)
{
	register int n = io_xcopy(in, out, arg);
	io_getc(in);
	return n;
}
