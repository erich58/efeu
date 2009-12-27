/*	Konfigurationsdatei f�r Pixmap - Shell
	(c) 1994 Erich Fr�hst�ck
	A-1090 Wien, W�hringer Stra�e 64/6

	Version 3.0
*/

Ident = "Pixmap \- Shell"
Version = "1.0"
Copyright = "(c) 1995 Erich Fr�hst�ck"

PgmOpt("I dir", "IncPath=paste(PATHSEP,#1,IncPath)",
	"Suchpfad f�r Skriptfiles, Vorgabe $(IncPath:s)")
//PgmOpt("x", "TraceFlag=true", "Systemaufrufe protokollieren")
//PgmOpt("n", "ExecFlag=false", "Systemaufrufe nur anzeigen")
PgmOptArg("file", "Input=#1", "Skriptfile")
PgmVaArg("arg(s)", NULL, "Argument(e)")

msgtab {
	1, "$!: Datei $(#1:%#s) ist nicht lesbar.\n"
	2, "$!: $0: Illegales Zeichen $1.\n"
}
