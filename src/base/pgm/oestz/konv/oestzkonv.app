/*	Konfigurationsdatei f�r oestzkonv
	(c) 1994 Erich Fr�hst�ck
	A-1090 Wien, W�hringer Stra�e 64/6
*/

Ident = "Aggregatfile konvertieren"
Version = "2.0"
Copyright = "(c) 1995 Erich Fr�hst�ck"

PgmOpt("t", "OestzHflag=\"$0\"", "Aggregatfilebeschreibung ausgeben")
PgmOpt("s", "OestzSflag=S", "Selektionen ausgeben")
PgmOpt("g", "OestzGflag=G", "Gegenst�nde ausgeben")
PgmOpt("k", "OestzKflag=K", "Kriterien ausgeben")
PgmOpt("d file", "TeXout=#1", "TeX-File mit Dokumentation generieren")
PgmOpt("h file", "Header=#1", "Headerfile f�r Datenverarbeitung generieren")
PgmOpt("K kenn", "Kennung=#1", "Filekennung setzen")
PgmOpt("x", "TeXmode=false", "Datendefinitionsfile generieren")
PgmOpt("o db", "DB=#1", "Datenbankfile ausgeben")
PgmVaArg("file(s)", NULL, "Aggregatfile(s)")
PgmXArg("*=*", "List=paste(' ', List, #1)", NULL)
PgmOptArg("g=f", NULL, "Multiplikator \\fIf\\fR f�r Gegenstand \\fIg\\fR")

msgtab {
	1, "File $1: $2 Datens�tze ausgegeben.\n"
}
