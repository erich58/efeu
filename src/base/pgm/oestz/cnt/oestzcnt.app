/*	Konfigurationsdatei f�r vzcount
	(c) 1994 Erich Fr�hst�ck
	A-1090 Wien, W�hringer Stra�e 64/6
*/

Ident = "Auswerten der H�user und Wohnungsz�hlung"
Version = "1.0"
Copyright = "(c) 1993 Erich Fr�hst�ck"

PgmOpt("db", "dblist()", "Auflisten der Datenbanken")
PgmOpt("t titel", "set(Titel)", "Titel der Ausz�hlung")
PgmOpt("w", "Type='w'", "Wohnungen")
PgmOpt("p", "Type='p'", "Personen")
PgmOpt("s", "Type='s'", "Arbeitsst�tten")
PgmOpt("a", "Type='a'", "Ausl�nd. unselbst. Besch�ftigte")
PgmOpt("v", "Type='v'", "Verteilung des Wohnungsaufwandes")
PgmOpt("n", "Type='n'", "Verteilung der Nutzfl�che")
PgmOpt("r", "Reduce=true", "1-dimensionale Achsen unterdr�cken")
PgmOpt("l pattern", "List=#1", "Auflisten der Z�hlgruppen")
PgmArg("db", "DB=#1", "Datenbank")
PgmOptArg("aus", "Output=#1", "Ausgabefile")
PgmXArg("*=*", "Grpdef=paste(\" \", Grpdef, #1)", NULL)
PgmOptArg("name=value", NULL, "Achsendefinition")

msgtab {
	1, "$!: Unbekannte Datenbankdefinition $(#1:s).\n"
}
