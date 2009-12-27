/*	Dokumentation aus Sourcefile generieren
	(c) 2000 Erich Fr�hst�ck
	A-3423 St.Andr�/W�rdern, S�dtirolergasse 17-21/5
*/

Ident = "Dokumentation aus Sourcefile generieren"
Version = "1.0"
Copyright = "(c) 2000 Erich Fr�hst�ck"

PgmOpt("t", "Output=\"|efeudoc -t -\"", "Terminalausgabe")
PgmOpt("tex", "Output=\"|efeudoc -l -\"", "LaTeX-Aufbereitung")
PgmOpt("ps", "Output=\"|efeudoc -p -\"", "PostScript-Aufbereitung")
PgmOpt("man", "Output=\"|efeudoc -m -\"", "Handbuchsource")
PgmOpt("lp", "Output=\"|efeudoc -lp -\"", "Ausgabe zum Drucker")
PgmOpt("n name", "Name=#1", "Ausgabename")
PgmOpt("m mode", "Mode=#1", "Verarbeitungsmodus")
PgmOpt("s num", "Secnum=#1", "Handbuchabschnitt")
PgmOpt("l", "ListMode(iostd); exit(0);", "Verarbeitungsmodi auflisten")
PgmArg("ein", "Input=#1", "Eingabefile")
PgmOptArg("aus", "Output=#1", "Ausgabefile")

msgtab {
	1, "$!: Verarbeitungsmodus mu� angegeben werden.\n"
	2, "$!: Unbekannter Verarbeitungsmodus $(Mode:%#s).\n"
}
