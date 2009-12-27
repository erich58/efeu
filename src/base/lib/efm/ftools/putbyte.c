/*	Binäre Ausgabe von Zahlenwerten in maschinenunabhängiger Form
	(c) 1996 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6

	Version 0.6

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

#include <EFEU/ftools.h>

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
