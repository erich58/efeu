/*	Standardkonfiguration f�r EFEU - Shell
	(c) 1994 Erich Fr�hst�ck
	A-1090 Wien, W�hringer Stra�e 64/6

	Version 3.0
*/

PgmOptArg("file", "Input=#1", string !
ist der Name des Skriptfiles, da� vom Interpreter verarbeitet werden soll
!);

PgmVaArg("arg(s)", NULL, string !
sind Argumente und Optionen, die vom Skriptfile verarbeitet werden.
!);
