/*	Konfigurationsdatei für oestzkonv
	(c) 1994 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6
*/

Ident = "Aggregatfile konvertieren"
Version = "2.0"
Copyright = "(c) 1995 Erich Frühstück"

PgmOpt("t", "OestzHflag=\"$0\"", "Aggregatfilebeschreibung ausgeben")
PgmOpt("s", "OestzSflag=S", "Selektionen ausgeben")
PgmOpt("g", "OestzGflag=G", "Gegenstände ausgeben")
PgmOpt("k", "OestzKflag=K", "Kriterien ausgeben")
PgmOpt("d file", "TeXout=#1", "TeX-File mit Dokumentation generieren")
PgmOpt("h file", "Header=#1", "Headerfile für Datenverarbeitung generieren")
PgmOpt("K kenn", "Kennung=#1", "Filekennung setzen")
PgmOpt("x", "TeXmode=false", "Datendefinitionsfile generieren")
PgmOpt("o db", "DB=#1", "Datenbankfile ausgeben")
PgmVaArg("file(s)", NULL, "Aggregatfile(s)")
PgmXArg("*=*", "List=paste(' ', List, #1)", NULL)
PgmOptArg("g=f", NULL, "Multiplikator \\fIf\\fR für Gegenstand \\fIg\\fR")

msgtab {
	1, "File $1: $2 Datensätze ausgegeben.\n"
}
