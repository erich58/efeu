/*	Bin�re Ausgabe von Zahlenwerten in maschinenunabh�ngiger Form
	(c) 1996 Erich Fr�hst�ck
	A-1090 Wien, W�hringer Stra�e 64/6

	Version 0.6

$Description
Die Funktionen |put1byte|, |put2byte|, |put3byte| und |put4byte|
erlauben eine bin�re Ausgabe von Ganzzahlwerten in maschinenunabh�ngiger
Form. Dabei werden h�hersignifikante Bytes zuerst ausgegeben.

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
