/*	Konfigurationsdatei f�r Programmgenerierung
	(c) 1994 Erich Fr�hst�ck
	A-1090 Wien, W�hringer Stra�e 64/6

	Version 3.0
*/

Ident = "Sourcefilegenerierung"
Version = "1.0"
Copyright = "(c) 1995 Erich Fr�hst�ck"

IncPath = paste(PATHSEP, IncPath, EFEUTOP + "/lib/mksource");
IncPath = paste(PATHSEP, IncPath, getenv("ESHPATH"));
fmt_str = "%*s";
VerboseMode = 1;

PgmOpt("I dir", "IncPath=paste(PATHSEP,#1,IncPath)",
	"erweitert den Suchpfad f�r Skriptfiles um <dir>, der ist Startwert $(IncPath:%#s)")
PgmOpt("s", "VerboseMode=0",
	"Arbeitsschritte nicht protokollieren")
PgmOpt("V", "VerboseMode=1",
	"Arbeitsschritte kurz protokollieren")
PgmOpt("v", "VerboseMode=2",
	"Arbeitsschritte ausf�hrlich protokollieren")
PgmOpt("l", "MakeList=true",
	"Liste der zu generierenden Dateien")
PgmOpt("L name", "ListName=#1;MakeList=true",
	"Liste mit Namen <name> generieren")
PgmOpt("r", "MakeRule=true",
	"Makefileregeln generieren")
PgmOpt("x", "AllTarget=NULL;CleanTarget=NULL;DependTarget=NULL",
	"Keine Pseudoziele f�r Makefileregeln")
PgmOpt("all=name", "AllTarget=#1",
	"Name des Generierungszieles")
PgmOpt("clean=name", "CleanTarget=#1",
	"Name des L�schzieles")
PgmOpt("depend=name", "DependTarget=#1",
	"Name des Ziels f�r Abh�ngigkeitsregeln")
PgmOpt("lock", "LockFlag=true",
	"Filegenerierung sperren")
PgmOpt("unlock", "LockFlag=false",
	"hebt eine vordefinierte Sperre auf")
PgmOpt("d", "MakeDep=true",
	"Abh�ngigkeitsregeln generieren")
PgmOpt("D name", "DependName=#1;MakeDep=true",
	"Abh�ngigkeitsregel f�r <name> generieren")
PgmOpt("n name", "config name=$1",
	"Basisname f�r Standardnamen")
PgmOpt("h", "config hdr",
	"Headerfile mit Standardnamen generieren")
PgmOpt("c", "config src",
	"Sourcefile mit Standardnamen generieren")
PgmOpt("t", "LockFlag=true;config tex",
	"TeX-Dokumentation mit Standardnamen generieren, andere Files sperren")
PgmOpt("H hdr", "config hdr=$1",
	"Headerfile mit Namen <hdr> generieren")
PgmOpt("C src", "config src=$1",
	"Sourcefile mit Namen <src> generieren")
PgmOpt("T doc", "LockFlag=true;config tex=$1",
	"TeX-Dokumentation <doc> generieren, andere Files sperren")
PgmArg("name", "Template=#1", "Skriptfile")

msgtab {
	1, "Unbekannte Ausgabdefinition $1.\n"
}
