/*
:*:	Write characters to string buffer
:de:	Zeichen in Stringbuffer schreiben

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
The function |$1| writes the character <c> in the string buffer <sb>
and returns the character value. If nessesary, the string buffer
is expandet. In opposite to the macro |sb_putc| the position in the
string buffer is always synchronized.
:de:
Die Funktion |$1| schreibt das Zeichen <c> in das Zeichenfeld <sb> und
liefert das geschriebene Zeichen als Rückgabewert. Das Zeichenfeld
wird bei Bedarf mit |sb_expand| vergrößert.
Gegenüber dem Makro |sb_putc| führt |$1| immer eine Synchronisation
der Zeichenfeldgröße mit der aktuellen Position durch.
*/

int sb_put (int c, StrBuf *sb)
{
	if	(sb && c != EOF)
	{
		sb_expand(sb, 1);
		sb->nfree--;
		return sb->data[sb->pos++] = (unsigned char) c;
	}

	return EOF;
}

/*	Zeichen wiederholt ausgeben
*/

int sb_nputc (int c, StrBuf *sb, int n)
{
	if	(sb && c != EOF && n > 0)
	{
		memset(sb_expand(sb, n), c, n);
		sb->pos += n;
		sb->nfree -= n;
		return n;
	}

	return 0;
}


/*
:*:
The function |$1| writes the string <str> to the string buffer <sb>.
:de:
Die Funktion |$1| schreibt den String <str> in das Zeichenfeld <sb>.
*/

int sb_puts (const char *str, StrBuf *buf)
{
	if	(str && buf)
	{
		int n;

		for (n = 0; *str; n++, str++)
			sb_putc(*str, buf);

		return n;
	}

	return 0;
}

/*
:*:
The function |$1| is similar to |sb_puts|, but writes not more than <len>
bytes of the string <str> to the string buffer <sb>.
:de:
Die Funktion |$1| schreibt maximal <len> Zeichen des Strings <str> in das
Zeichenfeld <sb>.
*/

int sb_nputs (const char *str, size_t len, StrBuf *buf)
{
	if	(str && buf)
	{
		int n;

		for (n = 0; n < len && *str; n++, str++)
			sb_putc(*str, buf);

		return n;
	}

	return 0;
}

/*
:*:
The function |$1| writes the string <str> in reverse order
to the string buffer <sb>.
:de:
Die Funktion |$1| schreibt den String <str> in umgekehrter
Reihenfolge (das letzte Zeichen zuerst) in das Zeichenfeld <sb>.
*/

int sb_rputs (const char *str, StrBuf *buf)
{
	if	(str && buf)
	{
		int n = strlen(str);
		int k = n;

		while (k-- > 0)
			sb_putc(str[k], buf);

		return n;
	}
	else	return 0;
}


/*
$SeeAlso
\mref{strbuf(7)},
\mref{memalloc(7)}.
*/
