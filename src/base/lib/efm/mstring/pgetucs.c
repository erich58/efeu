/*
:*:extract unicode character from string
:de:Unicode Zeichen aus String bestimmen

$Copyright (C) 2008 Erich Fr�hst�ck
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

#include <EFEU/ioscan.h>
#include <EFEU/fmtkey.h>
#include <ctype.h>

#define	U1(c)	((c & 0x80) == 0)	/* Test auf 1-Byte Sequenz */
#define	U2(c)	((c & 0xe0) == 0xc0)	/* Test auf 2-Byte Sequenz */
#define U3(c)	((c & 0xf0) == 0xe0)	/* Test auf 3-Byte Sequenz */
#define	U4(c)	((c & 0xf8) == 0xf0)	/* Test auf 4-Byte Sequenz */

#define	V2(c)	((c & 0x1f) << 6)	/* Startbits f�r 2-Byte Sequenz */
#define	V3(c)	((c & 0xf) << 12)	/* Startbits f�r 3-Byte Sequenz */
#define	V4(c)	((c & 0x7) << 18)	/* Startbits f�r 4-Byte Sequenz */

#define	UF(c)	((c & 0xc0) == 0x80)	/* Test auf Folgezeichen */
#define	VF(c)	(c & 0x3f)		/* Datenbits des Folgezeichens */

static int32_t get_sub (char **ptr, int32_t val, int n)
{
	int c;
	
	if	(n == 0)
		return val;

	c = *(*ptr)++;

	if	(UF(c) && (val = get_sub(ptr, val << 6 | VF(c), n - 1)) >= 0)
		return val;

	(*ptr)--;
	return EOF;
}

/*
Die Funktion |$1| liefert das n�chste Unicode-Zeichen aus einem String,
mit wahlweise ISO 8859-15 (latin9) oder UTF-8 kodierten Zeichen.
Als Argument wird ein Zeiger auf die aktuelle Stringposition �bergeben,
der entsprechend weitergez�hlt wird. Das Argument <lim> bestimmt,
wieviele Zeichen maximal gelesen werden k�nnen. Damit l��t sich die Funktion
auch auf nicht mit 0 terminierte Zeichenketten anwenden. Bei
null-terminierten Zeichenfeldern kann ein beliebiger Wert |>=| 4 �bergeben
werden.

Zeichen mit Kodewert zwischen 128 ud 256, die nicht Teil einer UTF8-Kodierung
sind, werden als ISO 8859-15-Kodewert aufgefasst und entsprechend umgewandelt.
Falls f�r <lim> ein Wert |<=| 1 �bergeben wird, werden keine Multibyte-Zeichen
aufgel�st und die Funktion verh�lt sich wie eine reine Konvertierung von
ISO 8859-15 in Unicode.
$Notes
Da diese Funktion prim�r f�r internen Gebrauch bestimmt ist, werden
die folgenden Rahmenbedingungen vorraussgesetzt und nicht gepr�ft:
*	<p> ist ein g�ltiger Pointer.
*	|*|<p> zeigt nicht auf die Abschlussnull bzw. auf das erste Zeichen
	nach einem Datenfeld.
*/

int32_t pgetucs (char **p, size_t lim)
{
	int c = *(*p)++;
	int32_t val = 0;

	if	(U1(c))			return c;
	else if	(U2(c) && lim > 1)	val = get_sub(p, V2(c), 1);
	else if	(U3(c) && lim > 2)	val = get_sub(p, V3(c), 2);
	else if	(U4(c) && lim > 3)	val = get_sub(p, V4(c), 3);
	else				return latin9_to_ucs(c);

	return val >= 0 ? val : latin9_to_ucs(c);
}
