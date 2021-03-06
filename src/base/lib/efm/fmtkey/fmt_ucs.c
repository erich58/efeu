/*
Konvertierung einer UCS-Zeichendefinition

$Copyright (C) 2007, 2008 Erich Frühstück
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

int fmt_ucs (StrBuf *sb, const FmtKey *key, int32_t c)
{
	int pos = sb->pos;
	int n = 0;

	if	(key->flags & FMT_ALTMODE)
	{
		n += sb_nputc('\'', sb, 1);

		if	(c >= 0x80)
		{
			sb_putucs(c, sb);
			n++;
		}
		else	n += sb_xputc(c, sb, "'");

		n += sb_nputc('\'', sb, 1);
	}
	else
	{
		sb_putucs(c, sb);
		n++;
	}

	return ftool_align(sb, key, pos, n);
}
