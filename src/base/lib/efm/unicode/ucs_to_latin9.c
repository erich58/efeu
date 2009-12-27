/*
Konvertierung von UCS-Zeichen

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

#include <EFEU/io.h>
#include <EFEU/locale.h>
#include <EFEU/mstring.h>

int ucs_to_latin9 (int32_t c)
{
	switch (c)
	{
	case 0x20AC: return 0xA4; /* EURO SIGN */
	case 0x0160: return 0xA6; /* LATIN CAPITAL LETTER S WITH CARON */
	case 0x0161: return 0xA8; /* LATIN SMALL LETTER S WITH CARON */
	case 0x017D: return 0xB4; /* LATIN CAPITAL LETTER Z WITH CARON */
	case 0x017E: return 0xB8; /* LATIN SMALL LETTER Z WITH CARON */
	case 0x0152: return 0xBC; /* LATIN CAPITAL LIGATURE OE */
	case 0x0153: return 0xBD; /* LATIN SMALL LIGATURE OE */
	case 0x0178: return 0xBE; /* LATIN CAPITAL LETTER Y WITH DIAERESIS */
	default:     break;
	}

	if	(c < 0)		return EOF;
	else if	(c > 0xff)	return '?';
	else			return c;
}
