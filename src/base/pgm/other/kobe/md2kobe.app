/*	Konfigurationsdatei für md2kobe
	(c) 1995 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6
*/

Ident = "Datenmatrix in KOBE-Datenbank umwandeln"
Version = "4.0"
Copyright = "(c) 1995 Erich Frühstück"

PgmOpt("c", "Cflag=true", "Kummulierte Bestandswerte zurückrechnen")
PgmOpt("m", "DatFmt='m'", "Datum auf Monatsbasis")
PgmOpt("x", "DatFmt='x'", "Datum im Zeitreihenstandard")
PgmArg("ein", "Input=#1", "Eingabefile")
PgmArg("aus", "Output=#1", "Ausgabefile")

msgtab {
	1, "$!: Datentype $(#1) hat falsche Bytelänge.\n"
	2, "$!: Datenmatrix hat zuwenig Achsen.\n"
	3, "$!: Achse mit Klasseneinteilung hat Dimension 0.\n"
}
