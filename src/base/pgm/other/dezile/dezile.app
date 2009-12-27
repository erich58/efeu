/*	Konfigurationsdatei für aldezile
	(c) 1994 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6
*/

Ident = "Deziltabellen generieren"
Version = "3.1"
Copyright = "(c) 1998 Erich Frühstück"

Output = "|mdprint -b -";
Mode = "quart";

PgmOpt("r", "Reduce=true", "Entfernen von singulären Achsen")
PgmOpt("s", "Spline=true", "Splineinterpolation der Lorenzkurve")
PgmOpt("m", "Mittel=true", "Mittelwerte berechnen")
PgmOpt("g", "Gini=true", "Gini-Koeffizient berechnen")
PgmOpt("v def", "Mode=#1", "Verteilungsmodus, Vorgabe $(Mode:%#s)")
PgmOpt("l mask", "LKMask=#1",
	"Filemaske für Lorenzkurven, Vorgabe $(LKMask:%#s)")
PgmArg("ein", "Input=#1", "Eingabefile")
PgmOptArg("aus", "Output=#1", "Ausgabefile")
PgmXArg("*=*", "SelDef=paste(' ', SelDef, #1)", NULL)
PgmOptArg("name=var", NULL, "Selektionsparameter")

msgtab {
	1, "$!: Datentype $1 hat falsche Bytelänge.\n"
	2, "$!: Datenmatrix hat keine Achsen.\n"
	3, "$!: Achse mit Klasseneinteilung hat Dimension 0.\n"
	5, "$!: Verteilungsmodus $(Mode:%#s) ist nicht definiert.\n"
}
