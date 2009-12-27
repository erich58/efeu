/*
IO-Strukturen umkopieren

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

#include <EFEU/parsub.h>

int iocopystr (const char *fmt, io_t *out, iocopy_t copy, void *arg)
{
	io_t *in;
	int n;

	in = io_cstr(fmt);
	n = copy(in, out, arg);
	io_close(in);
	return n;
}


char *miocopy (io_t *in, iocopy_t copy, void *arg)
{
	strbuf_t *sb;
	io_t *out;

	sb = new_strbuf(0);
	out = io_strbuf(sb);
	copy(in, out, arg);
	io_close(out);
	return sb2str(sb);
}


char *miocopystr (const char *fmt, iocopy_t copy, void *arg)
{
	strbuf_t *sb;
	io_t *in, *out;

	sb = new_strbuf(0);
	in = io_cstr(fmt);
	out = io_strbuf(sb);
	copy(in, out, arg);
	io_close(out);
	io_close(in);
	return sb2str(sb);
}
