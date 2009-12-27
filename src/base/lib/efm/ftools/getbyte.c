/*	Binäre Eingabe von Zahlenwerten in maschinenunabhängiger Form
	(c) 1996 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6

	Version 0.6

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


#include <EFEU/ftools.h>

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
	uchar_t byte[2];

	if	(fread(byte, 1, 2, file) != 2)
		input_error();

	return ((byte[0] << 8) + byte[1]);
}

unsigned get3byte (FILE *file)
{
	uchar_t byte[3];

	if	(fread(byte, 1, 3, file) != 3)
		input_error();

	return ((((byte[0] << 8) + byte[1]) << 8) + byte[2]);
}

unsigned get4byte (FILE *file)
{
	uchar_t byte[4];

	if	(fread(byte, 1, 4, file) != 4)
		input_error();

	return ((((((byte[0] << 8) + byte[1]) << 8) + byte[2]) << 8) + byte[3]);
}
