/*
Kopieren eines Strings mit Speicherzuweisung

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

/*
Die Funktion |$1| kopiert den String <s>.
Wurde als Argument ein Nullpointer übergeben, liefert die Funktion
ebenfalls einen Nullpointer.
*/

char *mstrcpy (const char *s)
{
	return s ? mstrncpy(s, strlen(s)) : NULL;
}

/*
Die Funktion |$1| kopiert
maximal <len> Zeichen des Strings <s>. Wurde als Argument ein Nullpointer
übergeben, liefert die Funktion einen Leerstring.
*/

char *mstrncpy (const char *s, size_t len)
{
	char *a;
	size_t i;

	a = (char *) memalloc(len + 1);
	i = 0;

	if	(s != NULL)
		while (i < len && *s != 0)
			a[i++] = *(s++);

	a[i] = 0;
	return a;
}

/*
$Notes
Das Speicherfeld für den Rückgabewert wird mit |memalloc| eingerichtet.
$SeeAlso
\mref{memalloc(3)}.\br
\mref{string(3C)} @PRM.
*/
