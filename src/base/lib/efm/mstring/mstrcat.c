/*
Zusammenfügen einer variablen Liste von Strings

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

#include <EFEU/mstring.h>
#include <EFEU/strbuf.h>

/*
Die Funktion |$1| dient zum Zusammenhängen beliebig vieler Strings
mit Trennzeichen <delim>.
Die variable Argumentliste wird mit einem Nullpointer abgeschlossen.
*/

char *mstrcat(const char *delim, const char *list, ...)
{
	const char *x;
	char *s;
	StrBuf *sb;
	va_list k;

	sb = sb_acquire();
	va_start(k, list);
	x = NULL;

	for (s = (char *) list; s != NULL; s = va_arg(k, char *))
	{
		sb_puts(x, sb);
		sb_puts(s, sb);
		x = delim;
	}

	va_end(k);
	return sb_cpyrelease(sb);
}


/*
Die Funktion <$1> hängt die beiden Strings <a> und <b>
mit dem Trennzeichen <delim> zusammen. Falls für einen beiden Strings
ein Nullpointer übergeben wurde, wird kein Trennzeichen ausgegeben.
Sind beide Strings Nullpointer, liefert die Funktion einen Nullpointer.
*/

char *mstrpaste(const char *delim, const char *a, const char *b)
{
	StrBuf *sb;

	if	(a == NULL)	return mstrcpy(b);
	if	(b == NULL)	return mstrcpy(a);

	sb = sb_acquire();
	sb_puts(a, sb);
	sb_puts(delim, sb);
	sb_puts(b, sb);
	return sb_cpyrelease(sb);
}

/*
$Notes
Das Speicherfeld für den Rückgabewert wird mit |memalloc| eingerichtet.
$SeeAlso
\mref{memalloc(3)}.\br
\mref{string(3C)} @PRM.
*/
