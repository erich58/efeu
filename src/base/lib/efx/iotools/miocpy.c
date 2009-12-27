/*
Umkopieren auf String

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


/*	Parametersubstitution
*/

char *miocpy(io_t *in, iocpy_t *def, size_t dim)
{
	strbuf_t *sb;
	io_t *tmp;

	sb = new_strbuf(0);
	tmp = io_strbuf(sb);
	iocpy(in, tmp, def, dim);
	io_close(tmp);

	if	(iocpy_flag)
		sb_putc(0, sb);

	return sb2str(sb);
}


char *miocpyfmt(const char *fmt, iocpy_t *def, size_t dim)
{
	io_t *in;
	char *p;

	in = io_cstr(fmt);
	p = miocpy(in, def, dim);
	io_close(in);
	return p;
}
