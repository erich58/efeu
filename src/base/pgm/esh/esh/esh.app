/*	Konfigurationsdatei f�r EFEU - Shell
	(c) 1994 Erich Fr�hst�ck
	A-1090 Wien, W�hringer Stra�e 64/6

	Version 3.0
*/

Ident = "EFEU--Shell"
Version = "1.0"
Copyright = "(c) 1995 Erich Fr�hst�ck"

//IncPath = ".:/home/efeu/include"
IncPath = paste(PATHSEP, IncPath, getenv("ESHPATH"));
CmdType = "esh";

PgmOpt("I dir", "IncPath=paste(PATHSEP,#1,IncPath)", string !
erweitert den Suchpfad f�r Skriptfiles um <dir>.
Die Vorgabe f�r den Suchpfad ist $(IncPath:%#s).
!);

PgmOpt("D name[=val]", string !
copy(preproc("#define " + strsub(#1, "=", " ")), NULL);
!, string !
definiert den Makro name mit Wert val.
!);

PgmOpt("noeval", "PPOnly=true", string !
Es wird nur der Preprozessor verwendet, das Skriptfile wird nicht
verarbeitet.
!);

PgmOpt("depend", "MakeDepend=true", string !
Es wird eine Abh�ngigkeitsliste zusammengestellt. Zieldateien werden
nicht generiert.
!);

