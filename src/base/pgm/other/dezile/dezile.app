/*	Konfigurationsdatei f�r aldezile
	(c) 1994 Erich Fr�hst�ck
	A-1090 Wien, W�hringer Stra�e 64/6
*/

Ident = "Deziltabellen generieren"
Version = "3.1"
Copyright = "(c) 1998 Erich Fr�hst�ck"

Output = "|mdprint -b -";
Mode = "quart";

PgmOpt("r", "Reduce=true", "Entfernen von singul�ren Achsen")
PgmOpt("s", "Spline=true", "Splineinterpolation der Lorenzkurve")
PgmOpt("m", "Mittel=true", "Mittelwerte berechnen")
PgmOpt("g", "Gini=true", "Gini-Koeffizient berechnen")
PgmOpt("v def", "Mode=#1", "Verteilungsmodus, Vorgabe $(Mode:%#s)")
PgmOpt("l mask", "LKMask=#1",
	"Filemaske f�r Lorenzkurven, Vorgabe $(LKMask:%#s)")
PgmArg("ein", "Input=#1", "Eingabefile")
PgmOptArg("aus", "Output=#1", "Ausgabefile")
PgmXArg("*=*", "SelDef=paste(' ', SelDef, #1)", NULL)
PgmOptArg("name=var", NULL, "Selektionsparameter")

msgtab {
	1, "$!: Datentype $1 hat falsche Bytel�nge.\n"
	2, "$!: Datenmatrix hat keine Achsen.\n"
	3, "$!: Achse mit Klasseneinteilung hat Dimension 0.\n"
	5, "$!: Verteilungsmodus $(Mode:%#s) ist nicht definiert.\n"
}
