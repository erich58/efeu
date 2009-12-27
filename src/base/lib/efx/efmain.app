/*	Initialisierungssequenzen für efmain - Programmbibliothek
	(c) 1994 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6
*/

setlocale(LC_DATE, "de")

//	Hilfetextparameter

HelpFmt = "\
@name
\\Name
@ident
\\Synopsis
@synopsis
\\Description
Folgende Optionen und Argumente werden vom Kommando |$!| akzeptiert:
@arglist 12
\\Environment
@environ 12
";

VersionFmt = "\
\\hang
|$!| - $(Ident:%s), Version $(Version:%s)
\\br
$(Copyright:%s)
";

InfoPath = ".:/efeu/lib/eis:/efeu/lib/efeu/info"

PgmEnv("SHELL", "Shell=#1",
	"legt die Shell für Systemaufrufe fest.");
PgmEnv("PAGER", "Pager='|'+#1",
	"bestimmt das Kommando für den Seitenfilter.");
PgmEnv("APPLPATH", "ApplPath=paste(':',ApplPath,#1)",
	"definiert den Suchpfad für Anpassungsfiles.");
PgmEnv("MessageHandler", "MessageHandler=#1",
	"definiert das Kommando zur Ausgabe von Fehlermeldungen.");

str help_command = "help_$1 ? eval(help_$1) : usage(NULL, NULL)";
str help_raw = "usage(NULL, iostd)";
str help_plain = "usage(NULL, \"|efeudoc -T - -\")";
str help_term = "usage(NULL, \"|efeudoc -T term - | less -r\")";
str help_test = "usage(NULL, \"|efeudoc -T test -\")";
str help_sgml = "usage(NULL, \"|efeudoc -T sgml -\")";
str help_html = "usage(NULL, \"|efeudoc -T html -\")";
str help_cgi = "usage(NULL, \"|efeudoc -T cgi -\")";
str help_roff = "usage(NULL, \"|efeudoc -T man -\")";
str help_tex = "usage(NULL, \"|efeudoc -T SynTeX -\")";
str help_man = "usage(NULL, \"|efeudoc -man -\")";
str help_lp = "usage(NULL, \"|efeudoc -lp -\")";

PgmOpt("?", "usage(UsageFmt, ioerr); exit(0)", NULL);
PgmOpt("help", "eval(help_term); exit(0)", NULL);
PgmOpt("help=type", "eval(psub(help_command)); exit(0)", string !
gibt den Handbucheintrag des Kommandos aus.
Der zusätzliche Parameter <type> bestimmt die Formatierung und
die Ausgabe des Handbucheintrags.
[term]	Terminalausgabe (default)
[raw]	Rohformat für |efeudoc|
[lp]	Ausgabe zum Drucker
!);

PgmOpt("info", "Info(NULL, iostd); exit(0)", NULL);
PgmOpt("info=entry", "Info(#1, iostd); exit(0)", string !
listet verfügbare Informationseinträge des Kommandos auf.
Das Argument <entry> ist optional.
!);

PgmOpt("dump", "InfoDump(NULL, iostd); exit(0)", NULL);
PgmOpt("dump=entry", "InfoDump(#1, iostd); exit(0)", string !
schreibt sämtliche Informationseinträge des Kommandos zur Standardausgabe.
Das optionale Argument <entry> erlaubt eine Teilselektion
der Einträge.
!);

PgmOpt("V", "usage(VersionFmt, ioerr); exit(0)", string !
gibt die Versionsnummer des Kommandos aus.
!);

