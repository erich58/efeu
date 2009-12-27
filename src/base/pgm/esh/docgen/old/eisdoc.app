/*	Dokumentformatierung
	(c) 1998 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6
*/

Ident = "Dokumentformatierung"
Version = "1.0"
Copyright = "(c) 1998, Erich Frühstück"

IncPath = ".:/local/lib/eis"

@load{eisdoc.cfg}

PgmEnv("EISCFG", "IncPath=paste(PATHSEP,#1,IncPath)",
	"Suchpfad für Skriptfiles, Vorgabe $(IncPath:%#s)");
PgmOpt("I dir", "IncPath=paste(PATHSEP,#1,IncPath)",
	"Suchpfad für Skriptfiles, Vorgabe $(IncPath:%#s)")
PgmOpt("d", "MakeDepend=true", "Abhängigkeitsregeln generieren");
PgmOpt("s", "VerboseMode=0", "Stille Verarbeitung");
PgmOpt("V", "VerboseMode=2", "Reduziertes Protokoll");
PgmOpt("v", "VerboseMode=2", "Vollständiges Protokoll");
PgmArg("ein", "Input=#1", "Eingabefile");
PgmOptArg("aus", "Output=#1", "Ausgabefile");
PgmXArg("*=*", "AddVar(#1)", NULL)
PgmOptArg("name=value", NULL, "Variablendefinition")
