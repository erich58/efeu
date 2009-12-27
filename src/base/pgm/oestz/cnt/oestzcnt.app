/*	Konfigurationsdatei für vzcount
	(c) 1994 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6
*/

Ident = "Auswerten der Häuser und Wohnungszählung"
Version = "1.0"
Copyright = "(c) 1993 Erich Frühstück"

PgmOpt("db", "dblist()", "Auflisten der Datenbanken")
PgmOpt("t titel", "set(Titel)", "Titel der Auszählung")
PgmOpt("w", "Type='w'", "Wohnungen")
PgmOpt("p", "Type='p'", "Personen")
PgmOpt("s", "Type='s'", "Arbeitsstätten")
PgmOpt("a", "Type='a'", "Ausländ. unselbst. Beschäftigte")
PgmOpt("v", "Type='v'", "Verteilung des Wohnungsaufwandes")
PgmOpt("n", "Type='n'", "Verteilung der Nutzfläche")
PgmOpt("r", "Reduce=true", "1-dimensionale Achsen unterdrücken")
PgmOpt("l pattern", "List=#1", "Auflisten der Zählgruppen")
PgmArg("db", "DB=#1", "Datenbank")
PgmOptArg("aus", "Output=#1", "Ausgabefile")
PgmXArg("*=*", "Grpdef=paste(\" \", Grpdef, #1)", NULL)
PgmOptArg("name=value", NULL, "Achsendefinition")

msgtab {
	1, "$!: Unbekannte Datenbankdefinition $(#1:s).\n"
}
