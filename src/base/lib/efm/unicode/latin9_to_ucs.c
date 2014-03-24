/*
Latin9 in UCS Konvertieren

$Copyright (C) 2008 Erich Frühstück
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

int32_t latin9_to_ucs (int c)
{
	switch (c)
	{
	case EOF:  return EOF;
	case 0xA4: return 0x20AC; /* EURO SIGN */
	case 0xA6: return 0x0160; /* LATIN CAPITAL LETTER S WITH CARON */
	case 0xA8: return 0x0161; /* LATIN SMALL LETTER S WITH CARON */
	case 0xB4: return 0x017D; /* LATIN CAPITAL LETTER Z WITH CARON */
	case 0xB8: return 0x017E; /* LATIN SMALL LETTER Z WITH CARON */
	case 0xBC: return 0x0152; /* LATIN CAPITAL LIGATURE OE */
	case 0xBD: return 0x0153; /* LATIN SMALL LIGATURE OE */
	case 0xBE: return 0x0178; /* LATIN CAPITAL LETTER Y WITH DIAERESIS */
	default:   return (unsigned char) c;
	}
}
