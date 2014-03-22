/*
:*:write unicode character as utf-8 character sequence to string
:de:Unicode Zeichen mit UTF-8 Codierung in String schreiben.

$Copyright (C) 2009 Erich Frühstück
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

/*
Die Funktion |$1| schreibt das Unicode-Zeichen als UTF-8 Sequenz in
den Stringbuffer <buf> der Länge <len> und liefert die Zahl der
ausgegebenen Bytes. Falls <buf> ein Nullpointer ist oder nicht
genügend Speicherplatz für die Codesequenz und eine Abschlußnull bietet,
wird kein Zeichen ausgegeben und die Funktion liefert die Zahl der benötigten
Bytes. Die Abschlußnull wird nicht geschrieben, sondern nur bei der
Speicherbedarfsrechnung berücksichtigt.
Zeichen mit negativen Codewert (z.B.: EOF) oder mit einem Codwert größer
als |0x001FFFFF| werden ignoriert.
*/

int nputucs (int32_t c, unsigned char *buf, size_t len)
{
	if	(c < 0)
	{
		return 0;
	}
	else if	(c <= 0x0000007F)
	{
		if	(buf && len > 1)
		{
			*buf = c;
		}

		return 1;
	}
	else if	(c <= 0x000007FF)
	{
		if	(buf && len > 2)
		{
			*buf++ = 0xc0 | ((c >> 6) & 0x1f);
			*buf = 0x80 | (c & 0x3f);
		}

		return 2;
	}
	else if	(c <= 0x0000FFFF)
	{
		if	(buf && len > 3)
		{
			*buf++ = 0xe0 | ((c >> 12) & 0x0f);
			*buf++ = 0x80 | ((c >> 6) & 0x3f);
			*buf = 0x80 | (c & 0x3f);
		}

		return 3;
	}
	else if	(c <= 0x001FFFFF)
	{
		if	(buf && len > 3)
		{
			*buf++ = 0xf0 | ((c >> 18) & 0x07);
			*buf++ = 0x80 | ((c >> 12) & 0x3f);
			*buf++ = 0x80 | ((c >> 6) & 0x3f);
			*buf = 0x80 | (c & 0x3f);
		}

		return 4;
	}
	else 	return 0;
}

/*
$SeeAlso
\mref{pgetucs(3)}.
*/
