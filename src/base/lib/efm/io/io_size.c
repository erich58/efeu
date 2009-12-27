/*
Kompakte Ein- Ausgabe von Größenangaben

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


#include <EFEU/io.h>

/*
$Description

Die kompakte Darstellung einer Größenangabe erlaubt eine Dichte
Darstellung von Datenwerten unterschiedlicher Größenordnung.
Die Zahl der benötigten Byte hängt vom Größenwert ab.

Sie wird folgendermaßen gebildet:
Der Zahlenwert wird zunächst in 7-Bit Tupel zerlegt (Basis 128).
Diese werden beginnend beim niedrigsignifikantesten Wert ausgegeben,
wobei das 8. Bit als Flag dient und anzeigt, ob noch ein zusätzliches Tupel
benötigt wird.

Die folgende Tabelle zeigt Beispielhaft die Umkodierung:
---- verbatim
0	00000000
1	00000001
...
127	01111111
128	10000000 00000001
129	10000001 00000001
...
16383	11111111 01111111
16384	10000000 10000000 00000001
16385	10000001 10000000 00000001
...
----
*/

#define	SHIFT	7
#define	M_FLAG	(1 << SHIFT)
#define	M_DATA	(0xFF & ~M_FLAG)

/*
Die Funktion |$1| liest eine kompakte Größenangabe
aus der Eingabestruktur <io>.
*/

size_t io_getsize (IO *io)
{
	int c;
	
	if	((c = io_getc(io)) == EOF)
		return 0;

	return (c & M_FLAG) ? (io_getsize(io) << SHIFT) + (c & M_DATA) : c;
}

/*
Die Funktion |$1| schreibt eine Größenangabe
in kompakter Form in die Ausgabestruktur <io>.

$Diagnostic
Die Funktion |$1| liefert die Zahl der ausgegebenen Byte oder
EOF bei einem Ausgabefehler.
*/

int io_putsize (size_t val, IO *io)
{
	if	(io && io->put)
	{
		int n = 0;

		while (val > M_DATA)
		{
			io->put(M_FLAG | (val & M_DATA), io->data);
			val >>= SHIFT;
			n++;
		}

		io->put(val, io->data);
		return n + 1;
	}
	else	return 0;
}
