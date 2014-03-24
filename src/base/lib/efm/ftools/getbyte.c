/*
Binäre Eingabe von Zahlenwerten in maschinenunabhängiger Form

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
$Description
Die Funktionen |get1byte|, |get2byte| und |get4byte|
erlauben eine binäre Eingabe von Ganzzahlwerten in maschinenunabhängiger
Form der Längen 1, 2, 3 und 4 Byte.
Dabei wird davon ausgegangen, daß höhersignifikante Bytes
zuerst gespeichert werden. Die Funktionen sind mit den
Ausgabefunktionen |put1byte|, |put2byte|, |put3byte| und |put4byte|
kompatibel (Vergleiche dazu \mref{putbyte(3)}).

$Diagnostic
Falls ein Lesefehler auftritt wird eine Fehlermeldung ausgegeben
und das Programm abgebrochen.

$SeeAlso
\mref{putbyte(3)}.
*/


static void input_error(void)
{
	fprintf(stderr, "getxbyte: Unerwartetes Fileende.\n");
	exit(1);
}

unsigned get1byte (FILE *file)
{
	register int val;
	
	if	((val = getc(file)) == EOF)	input_error();

	return val;
}

unsigned get2byte (FILE *file)
{
	unsigned char byte[2];

	if	(fread(byte, 1, 2, file) != 2)
		input_error();

	return ((byte[0] << 8) + byte[1]);
}

unsigned get3byte (FILE *file)
{
	unsigned char byte[3];

	if	(fread(byte, 1, 3, file) != 3)
		input_error();

	return ((((byte[0] << 8) + byte[1]) << 8) + byte[2]);
}

unsigned get4byte (FILE *file)
{
	unsigned char byte[4];

	if	(fread(byte, 1, 4, file) != 4)
		input_error();

	return ((((((byte[0] << 8) + byte[1]) << 8) + byte[2]) << 8) + byte[3]);
}
