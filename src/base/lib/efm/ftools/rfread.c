/*
Binäre Ein- Ausgabe mit reverser Byteordnung

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


#include <EFEU/ftools.h>

/*
Die Funktion |$1| liest <nitems> Datensätze der Länge <size>
aus der Datei <stream> in den Buffer <ptr>,
wobei jeder Datensatz in umgekehrter Reihenfolge gelesen.
*/

size_t rfread (void *ptr, size_t size, size_t nitems, FILE *file)
{
	nitems = fread(ptr, size, nitems, file);

	if	(size > 1)
	{
		size_t n, k;
		char c, *p;

		p = (char *) ptr;

		for (n = 0; n < nitems; n++)
		{
			for (k = (size + 1) / 2; k < size; k++)
			{
				c = p[k];
				p[k] = p[size - k - 1];
				p[size - k - 1] = c;
			}

			p += size;
		}
	}

	return nitems;
}

/*
Die Funktion |$1| schreibt <nitems> Datensätze der Länge <size>
vom Buffer <ptr> in die Datei <stream>,
wobei jeder Datensatz in umgekehrter Reihenfolge ausgegeben wird.
*/

size_t rfwrite (const void *ptr, size_t size, size_t nitems, FILE *file)
{
	if	(size > 1)
	{
		size_t n, k;
		const uchar_t *p;

		p = ptr;

		for (n = 0; n < nitems; n++)
		{
			k = size;

			while (k-- != 0)
				putc(p[k], file);

			if	(feof(file))	break;

			p += size;
		}

		return n;
	}
	else	return fwrite(ptr, size, nitems, file);
}

/*
$Note
Die Funktionen sind, abgesehen von der Bytereihenfolge eines Datensatzes,
mit |fread| und |fwrite| kompatibel.

$SeeAlso
\mref{fread(3S)} @PRM.
*/
