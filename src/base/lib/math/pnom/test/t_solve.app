/*	Konfigurationsdatei für tst
	(c) 1994 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6
*/

Ident = "Nullstellenberechnung von Polynomen"

PgmOpt("f val", "SolveEps=#1", "Genauigkeit, Vorgabe $(SolveEps)")
PgmOpt("n anz", "SolveStep=#1", "Maximale Stufenzahl, Vorgabe $(SolveStep)")
PgmOpt("a val", "x0=#1", "Intervallanfang, Vorgabe $(x0)")
PgmOpt("e val", "x1=#1", "Intervallende, Vorgabe $(x1)")
PgmOpt("s", "SolveFlag=true", "Protokoll unterdrücken")
PgmVaArg("c0 c1 ...", NULL, "Koeffizienten")

msgtab {
	1, "Nullstelle: %lf\n"
}

