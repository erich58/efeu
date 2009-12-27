/*	Dokumentation aus Sourcefile generieren
	(c) 2000 Erich Frühstück
	A-3423 St.Andrä/Wördern, Südtirolergasse 17-21/5
*/

Ident = "Dokumentation aus Sourcefile generieren"
Version = "1.0"
Copyright = "(c) 2000 Erich Frühstück"

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
	1, "$!: Verarbeitungsmodus muß angegeben werden.\n"
	2, "$!: Unbekannter Verarbeitungsmodus $(Mode:%#s).\n"
}
