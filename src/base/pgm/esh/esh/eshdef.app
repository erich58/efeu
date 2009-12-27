/*	Standardkonfiguration für EFEU - Shell
	(c) 1994 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6

	Version 3.0
*/

PgmOptArg("file", "Input=#1", string !
ist der Name des Skriptfiles, daß vom Interpreter verarbeitet werden soll
!);

PgmVaArg("arg(s)", NULL, string !
sind Argumente und Optionen, die vom Skriptfile verarbeitet werden.
!);
