/*
Printf für IO-Strukturen

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

#include <EFEU/mstring.h>
#include <EFEU/io.h>


char *msprintf(const char *fmt, ...)
{
	va_list args;
	char *p;

	va_start(args, fmt);
	p = mvsprintf(fmt, args);
	va_end(args);
	return p;
}


char *mvsprintf(const char *fmt, va_list list)
{
	strbuf_t *sb;
	io_t *io;

	sb = new_strbuf(0);
	io = io_strbuf(sb);
	io_vprintf(io, fmt, list);
	io_close(io);
	return sb2str(sb);
}
