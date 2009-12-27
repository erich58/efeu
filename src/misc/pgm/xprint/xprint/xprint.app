/*	Konfigurationsdatei für EFEU - Shell
	(c) 1994 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6

	Version 3.0
*/

Ident = "Formatierung von Handbucheinträgen"
Version = "1.0"
Copyright = "(c) 1995 Erich Frühstück"

PCPath = EFEUTOP + "/lib/xprint:."

Type = getenv("PRINTER", "hft");

if	(Type == "PRINTER")	Type="hft"

LeftMargin = 5;
TopMargin = 2;
PageWidth = 60;
PageHeight = 30;
HeadHeight = 3;
FootHeight = 3;

LeftHead = "$N";
CenterHead = NULL;
RightHead = "$D $T";
LeftFoot = NULL;
CenterFoot = NULL;
RightFoot = "$#";

PgmOpt("s range", "Range=#1", "Seitenbereich")
PgmOpt("P type", "Type=#1", "Druckertype, Vorgabe $(Type)")
PgmOpt("p", "Type=\"560C\"", "Standarddrucker")
PgmOpt("c", "Mode=\"ctrl\"", "Druckmodus")
PgmOpt("m mode", "Mode=#1", "Druckmodus, Vorgabe $(Mode)")
PgmOpt("f fmt", "Format=#1", "Ausgabeformat")
PgmOpt("t titel", "Title=#1", "Titel")
PgmVaArg("file(s)", NULL, "Datei(en)")

msgtab {
	1,	"Drucker $(#1:%#s) nicht gefinden.\n"
	2,	"Leere Datei.\n"
}
