/*
Konvertierung einer UCS-Zeichendefinition

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

#include <EFEU/fmtkey.h>


int fmt_ucs (IO *io, const FmtKey *key, int32_t c)
{
	int n = 0;
	int k = 0;

	if	(key->flags & FMT_ALTMODE)
	{
		n += io_nputc('L', io, 1);
		n += io_nputc('\'', io, 1);

		if	(c < 0x80)
			n += io_xputc(c, io, "'");
		else	n += io_putucs_utf8(c, io);

		n += io_nputc('\'', io, 1);
		return n;
	}

	if	((key->flags & FMT_RIGHT) && key->width > 0)
		n += io_nputc(' ', io, key->width - 1);

	k = io_putucs(c, io);
	n += io_nputc(' ', io, key->width - n - 1);
	return n + k;
}
