/*
:*:	Get character from string buffer
:de:	Zeichen aus Stringbuffer lesen

$Copyright (C) 1996 Erich Frühstück
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

#include <EFEU/strbuf.h>


/*
:*:
The function |$1| reads the next character from the string field <sb>.
If the end of previously written data is reached, |EOF| is returned.
:de:
Die Funktion |$1| liest ein Zeichen aus dem Zeichenfeld <sb>.
Wurde das Ende des Zeichenfeldes erreicht, liefert er EOF.
Sie ist Funktionsgleich mit dem Makro |sb_getc| erlaubt aber
einen Nullpointer als Argument. In diesem Fall liefert sie immer EOF.
*/

int sb_get (StrBuf *sb)
{
	return sb ? sb_getc(sb) : EOF;
}


/*
$SeeAlso
\mref{strbuf(7)},
\mref{memalloc(7)}.
*/
