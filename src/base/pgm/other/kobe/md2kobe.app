/*	Konfigurationsdatei f�r md2kobe
	(c) 1995 Erich Fr�hst�ck
	A-1090 Wien, W�hringer Stra�e 64/6
*/

Ident = "Datenmatrix in KOBE-Datenbank umwandeln"
Version = "4.0"
Copyright = "(c) 1995 Erich Fr�hst�ck"

PgmOpt("c", "Cflag=true", "Kummulierte Bestandswerte zur�ckrechnen")
PgmOpt("m", "DatFmt='m'", "Datum auf Monatsbasis")
PgmOpt("x", "DatFmt='x'", "Datum im Zeitreihenstandard")
PgmArg("ein", "Input=#1", "Eingabefile")
PgmArg("aus", "Output=#1", "Ausgabefile")

msgtab {
	1, "$!: Datentype $(#1) hat falsche Bytel�nge.\n"
	2, "$!: Datenmatrix hat zuwenig Achsen.\n"
	3, "$!: Achse mit Klasseneinteilung hat Dimension 0.\n"
}
