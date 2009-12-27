/*	Konfigurationsdatei für EFEU - Shell
	(c) 1994 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6

	Version 3.0
*/

Ident = "EFEU--Shell"
Version = "1.0"
Copyright = "(c) 1995 Erich Frühstück"

//IncPath = ".:/home/efeu/include"
IncPath = paste(PATHSEP, IncPath, getenv("ESHPATH"));
CmdType = "esh";

PgmOpt("I dir", "IncPath=paste(PATHSEP,#1,IncPath)", string !
erweitert den Suchpfad für Skriptfiles um <dir>.
Die Vorgabe für den Suchpfad ist $(IncPath:%#s).
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
Es wird eine Abhängigkeitsliste zusammengestellt. Zieldateien werden
nicht generiert.
!);

