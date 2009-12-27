/*
ASCII-Konvertierungsfunktionen

$Copyright (C) 2001 Erich Frühstück
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

#include <EFEU/dbutil.h>

/*
Die Funktion |$1| ladet einen Datenbanksatz im Textformat
aus der Eingabestruktzur <io> in den Buffer <buf>. Das Argumnent
<recl> gibt die Satzlänge an. Der Buffer <buf> muß um mindestens 1 Byte
größer sein als die Satzlänge <recl>.

Falls die Datensatzzeile länger als die angegebene Satzlänge ist,
wird der Rest der Zeile ignoriert, falls die Datensatzzeile kürzer
als die Satzlänge ist, wird der Buffer mit Leerzeichen ergänzt.
Ein Wagenrücklauf in der Eingabedatei wird ignoriert.
Ein NUL-Zeichen wird durch ein Leerzeichen esrsetzt.

$Diagnostic
Bei einem Lesefehler oder beim Ende der Datei liefert die Funktion |$1| einen
Nullpointer. Ansonsten wird die Adresse von <buf> zurückgegeben.
*/

char *txt_load (IO *io, char *buf, int recl)
{
	int c, n;

	n = 0;

	while ((c = io_getc(io)) != '\n')
	{
		if	(c == EOF)	return NULL;
		if	(c == '\r')	continue;
		if	(c == 0)	c = ' ';
		if	(n < recl)	buf[n++] = c;
	}

	while (n < recl)
		buf[n++] = ' ';

	buf[n] = 0;
	return buf;
}

/*
$SeeAlso
\mref{txtval(3)}, \mref{dbutil(7)}.
*/
