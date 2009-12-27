/*	Konfigurationsdatei f�r efeudoc
	(c) 1999 Erich Fr�hst�ck
	A-3423 St.Andr�/W�rdern, S�dtirolergasse 17-21/5
*/

Ident = "EFEU-Dokumentkonverter"
Version = "1.0"
Copyright = "(c) 1999 Erich Fr�hst�ck"

Type = NULL;
Config = NULL;

PgmOpt("T type", "Type=#1", "Ausgabetype")
PgmOpt("C cfg", "Config=#1", "Konfigurationsfile")

PgmOpt("H hdr", "Head=#1", string !
Wertet den String <hdr> for den Eingabedateien aus.
Damit k�nnen Dokumentmakros definiert werden.
!);

PgmOpt("I dir", "IncPath=paste(PATHSEP,#1,IncPath)", string !
erweitert den Suchpfad f�r Skriptfiles um <dir>.
Die Vorgabe f�r den Suchpfad ist $(IncPath:%#s).
!);

PgmOpt("D name[=val]", string !
copy(preproc("#define " + strsub(#1, "=", " ")), NULL);
!, string !
definiert den esh-Makro name mit Wert val.
!);

PgmOpt("E expr", "cmdeval(#1, NULL, true)", "Wertet den esh-Ausdruck aus")

PgmOpt("0", "NoHead=true", "Dokumentkopf unterdr�cken")

PgmOpt("x", "Type=\"test\"", "Testausgabe")
PgmOpt("t", "Type=\"term\"; Output=\"|less -r\"", "Terminalausgabe")
PgmOpt("l", "Type=\"SynTeX\"", "LaTeX-Dokument im Synthesis-Stil")
PgmOpt("p", "Type=\"SynPS\"", "PostScript-Dokument im Synthesis-Stil")

PgmOpt("m", "Type=\"man\"", "Handbuchsource")
PgmOpt("n", "Type=\"nroff\"", "Nroff-formatiertes Handbuch")

PgmOpt("a", "Type=\"txt\"", "Textausgabe mit Attributen")
PgmOpt("f", "Type=\"filter\"", "Textausgabe ohne Attribute")

PgmOpt("s", "Type=\"sgml\"", "SGML-Dokument")
PgmOpt("h", "Type=\"html\"", "HTML-Dokument")
PgmOpt("L", "Type=\"latex\"", "LaTeX-Dokument �ber SGML")
PgmOpt("P", "Type=\"ps\"", "PostScript-Dokument �ber SGML")
PgmOpt("cgi", "Type=\"cgi\"", "HTML-Dokument mit CGI-Kopf")

PgmOpt("lp", "Type=\"SynPS\"; Output=\"|lpr\"", "Ausgabe zum Drucker")
PgmOpt("man", "Type=\"nroff\"; Output=\"|less\"", "Formatiertes Handbuch")
PgmOpt("txt", "Type=\"txt\"; Output=\"|less -r\"", "SGML-Textausgabe")
PgmOpt("o aus", "Output=#1", "Ausgabefile")

PgmVaArg("file(s)", NULL, "Eingabedatei(en)")

