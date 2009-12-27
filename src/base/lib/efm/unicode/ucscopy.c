/*
:*:copy unicode character sequences (latin or UTF-8) as utf-8 character
:de:Unicode-Zeichensequencen (codiert als Latin oder UTF-8) kopieren.

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
$Description
:*:The functions |ucscpy_utf8| and |ucscpy_latin9| converts the unicode
characters in <src> to the wanted character encoding. The source string <src>
may be coded either in utf-8 or latin9 as described in \mref{pgetucs(3)}.
The result is always terminated and the functions do not write more
than <size> bytes (including the trailing |'\0'|).
The function returns the number of Bytes necessary for the result (not
including the terminating |'\0'|).
A return value greater or equal <size> indicates, that the result was truncated.
A null pointer as <src> is handled like an empty string.
A null pointer as <tg> may be used to calculate the necessary bytes for
the conversion.
If <tg> is not a null pointer and <size> is greater then 0, there is always
written a terminating |'\0'| to <tg>.
:de:Die Funktionen |ucscpy_utf8| und |ucscpy_latin9| interpretieren den
String <src> als Unicode-Zeichensequenz und kopieren ihn in der gewünschten
Codesequenz. Der Ausgangsstring <src> kann wahlweise in latin9 oder UTF-8
kodiert sein. Vergleiche dazu \mref{pgetucs(3)}.
Die Funktionen liefern die Zahl der benötigten Zeichen bei vollständiger
Konvertierungi inklusive der abschließenden |'\0'|.
Ein Rückgabewert größer als <size> bedeutet also, das das
Reslultat abgeschnitten wurde.
Ein Nullpointer für <src> wird wie ein Leerstring behandelt.
Ein Nullpointer für <tg> ist zulässig. In diesem Fall errechnet die Funktion
nur die Zahl der benötigten Bytes.
Falls <tg> kein Nullpointer und <size> ungleich 0 ist, wird zumindest
die Abschlußnull in den Ausgabestring geschrieben.

:*:The function |$1| copies the source string <src> as utf-8 code sequence to
the target buffer of size <size>. The memory areas should not overlap.
:de:Die Funktion |$1| kopiert den Ausgangsstring <src>
als UTF-8 Sequenz in den Buffer <tg> der Länge <size>.
Die Speicherbereiche dürfen sich nicht überlappen.
*/

int ucscopy_utf8 (char *tg, size_t size, const char *src)
{
	int n, k;

	if	(!(src && *src))
	{
		if	(tg && size)
			*tg = 0;

		return 0;
	}

	for (n = 0; *src; n += k)
	{
	       	k = nputucs(pgetucs((char **) &src, 4),
			(unsigned char *) tg, size);

		if	(k < size)
		{
			tg += k;
			size -= k;
		}
		else if	(tg && size)
		{
			*tg = 0;
			tg = NULL;
			size = 0;
		}
	}

	if	(tg && size)
		*tg = 0;

	return n;
}

/*
:*:The function |$1| copies the source string as latin9 code sequence to
the target buffer of size <size>.
While the resluting string is always equal or less the original
string, the conversion may be done in place by using the same Pointer for
<tg> as for <src>. It is save to use |~0| for <size>.
:de:Die Funktion |$1| kopiert den Ausgangsstring
als UTF-8 Sequenz in den Stringbuffer <tg> der Länge <size>.
Da der resultierende String stets kleiner oder gleich dem Ausgangsstring ist,
erlaubt die Funktion die Konvertierung eines Strings "in place", indem für <tg>
der gleiche Pointer wie für <src> angegeben wird. Falls die Buffergröße
unbekannt ist, kann für <size> der Wert |~0| angegeben werden.
*/

int ucscopy_latin9 (char *tg, size_t size, const char *src)
{
	int n;
	int32_t c;

	if	(!(src && *src))
	{
		if	(tg && size)
			*tg = 0;

		return 0;
	}

	for (n = 0; *src; n++)
	{
	       	c = pgetucs((char **) &src, 4);

		if	(tg && size > 1)
		{
			*tg++ = ucs_to_latin9(c);
			size--;
		}
	}

	if	(tg && size)
		*tg = 0;

	return n;
}

/*
$SeeAlso
\mref{pgetucs(3)},
\mref{nputucs(3)}.
*/
