/*	Dokumentformatierung
	(c) 1999 Erich Fr�hst�ck
	A-3423 St.Andr�/W�rdern, S�dtirolergasse 17-21/5
*/

Ident = "Dokumentformatierung"
Version = "1.0"
Copyright = "(c) 1999, Erich Fr�hst�ck"

IncPath = ".:/local/lib/efeu/docgen:/home/efeu/include"
IncPath = paste(PATHSEP, IncPath, getenv("ESHPATH"));
Encoding = "std";
//Type = "term";
Encoding = NULL;
Type = "test";
VerboseMode = 0;

void cfg_man(void)
	DocBeg = "\\begin_document\n\n", DocEnd = "\\end_document\n";

PgmEnv("EISCFG", "IncPath=paste(PATHSEP,#1,IncPath)",
	"Suchpfad f�r Skriptfiles, Vorgabe $(IncPath:%#s)");
PgmOpt("I dir", "IncPath=paste(PATHSEP,#1,IncPath)",
	"Suchpfad f�r Skriptfiles, Vorgabe $(IncPath:%#s)")
PgmOpt("e enc", "Encoding=#1", "Eingabekonvertierung, Vorgabe $(Encoding)");
PgmOpt("t type", "Type = #1", "Formatierungstype, Vorgabe $(Type)");
PgmOpt("m macro", "MacList = paste(\" \", MacList, #1)", "Makropaket laden");
PgmOpt("d", "MakeDepend=true", "Abh�ngigkeitsregeln generieren");
PgmOpt("s", "VerboseMode=0", "Stille Verarbeitung");
PgmOpt("V", "VerboseMode=1", "Reduziertes Protokoll");
PgmOpt("v", "VerboseMode=2", "Vollst�ndiges Protokoll");
PgmOpt("x", "cfg_man()", "Dokumentkopf generieren");
PgmOptArg("ein", "Input=#1", "Eingabefile");
PgmOptArg("aus", "Output=#1", "Ausgabefile");
PgmXArg("*=*", "macdef(#1)", NULL)
PgmOptArg("name=value", NULL, "Definiert den Makro <name> mit Wert <value>")
