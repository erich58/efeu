/*
Binäre Ausgabe von Zahlenwerten in maschinenunabhängiger Form

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
Die Funktionen |put1byte|, |put2byte|, |put3byte| und |put4byte|
erlauben eine binäre Ausgabe von Ganzzahlwerten in maschinenunabhängiger
Form. Dabei werden höhersignifikante Bytes zuerst ausgegeben.

$Diagnostic
Falls ein Ausgabefehler auftritt wird eine Fehlermeldung ausgegeben
und das Programm abgebrochen.

$SeeAlso
\mref{getbyte(3)}.
*/

static void output_error(void)
{
	fprintf(stderr, "putxbyte: Ausgabefehler.\n");
	exit(1);
}

void put1byte (unsigned val, FILE *file)
{
	if	(putc((val) & 0xFF, file) == EOF)	output_error();
}

void put2byte (unsigned val, FILE *file)
{
	if	(putc((val >> 8) & 0xFF, file) == EOF)	output_error();
	if	(putc((val) & 0xFF, file) == EOF)	output_error();
}

void put3byte (unsigned val, FILE *file)
{
	if	(putc((val >> 16) & 0xFF, file) == EOF)	output_error();
	if	(putc((val >> 8) & 0xFF, file) == EOF)	output_error();
	if	(putc((val) & 0xFF, file) == EOF)	output_error();
}

void put4byte (unsigned val, FILE *file)
{
	if	(putc((val >> 24) & 0xFF, file) == EOF)	output_error();
	if	(putc((val >> 16) & 0xFF, file) == EOF)	output_error();
	if	(putc((val >> 8) & 0xFF, file) == EOF)	output_error();
	if	(putc((val) & 0xFF, file) == EOF)	output_error();
}
