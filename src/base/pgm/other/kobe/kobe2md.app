/*	Konfigurationsdatei für kobe2md
	(c) 1994 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6
*/

Ident = "KOBE-Datenbank aus Datenmatrix generieren"
Version = "4.0"
Copyright = "(c) 1995 Erich Frühstück"

PgmOpt("c", "Cflag=true", "Bestandswerte kummulieren")
PgmOpt("m", "DatFmt='m'", "Datum auf Monat abschneiden")
PgmOpt("x", "DatFmt='x'", "Datum im Zeitreuhenstandard ausgeben")
PgmOpt("n name", "Name=#1", "Achsenname der Personengruppen, Vorgabe $(Name)")
PgmOpt("t titel", "Title=#1", "Titel der Datenmatrix")
PgmOpt("z name", "DatName=#1", "Name der Zeitachse, Vorgabe $(DatName)")
PgmArg("db", "Input=#1", "KOBE-Datenbank")
PgmArg("aus", "Output=#1", "Ausgabefile")

msgtab {
	1, "$!: Eingabefile $(Input) ist keine KOBE-Datenbank.\n"
	2, "$!: KOBE-Datenbank $(Input) ist defekt.\n"
	3, "$!: Filepositionierung fehlgeschlagen.\n"
	4, "$!: Stichtage sind nicht kompatibel.\n"
}
