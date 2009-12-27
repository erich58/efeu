/*	Konfigurationsdatei für EFEU - Shell
	(c) 1994 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6

	Version 3.0
*/

Ident = "Formatierung von Handbucheinträgen"
Version = "1.0"
Copyright = "(c) 1995 Erich Frühstück"

str filter_nroff = "|nroff -Tlatin1 -man";
str filter_man = "|nroff -Tlatin1 -man | less -r";
str filter_lp = "|tex2ps - | lpr -h";
str filter_lpr = "|man2ps - | remsh beta -l lp lp";

Type = "term";
Filter = Pager;

PgmOpt("list", "Flag=true", "Befehle anzeigen")
PgmOpt("s num", "Init=psub(\"\\secnum[$1]\n\")", "Sektion setzen")
PgmOpt("test", "Type=\"test\";Filter=NULL", "Testausgabe")
PgmOpt("plain", "Type=\"plain\";Filter=NULL", "Plain-Ausgabe")
PgmOpt("tex", "Type=\"tex\";Filter=NULL", "LaTeX-Source")
PgmOpt("doc", "Type=\"doc\";Filter=NULL", "LaTeX-Source mit Header")
PgmOpt("lp", "Type=\"doc\";Filter=filter_lp", "Druckerausgabe mit \\LaTeX")
PgmOpt("lpr", "Type=\"roff\";Filter=filter_lpr", "Druckerausgabe mit troff")
PgmOpt("roff", "Type=\"roff\";Filter=NULL", "roff-Ausgabe")
PgmOpt("nroff", "Type=\"roff\";Filter=filter_nroff", "nroff-Formatierung")
PgmOpt("man", "Type=\"roff\";Filter=filter_man", "nroff-Formatierung mit Seitenfilter")
PgmOpt("term", "Type=\"term\";Filter=NULL", "Bildschirmausgabe")
PgmOpt("less", "Type=\"term\";Filter=Pager", "Bildschirmausgabe mit Seitenfilter")
PgmArg("ein", "Input=#1", "Eingabefile")
PgmOptArg("aus", "Output=#1", "Ausgabefile")
