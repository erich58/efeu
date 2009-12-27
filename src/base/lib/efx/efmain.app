/*
Initialisierungssequenzen für efmain - Programmbibliothek

$Copyright (C) 1994 Erich Frühstück
This file is part of EFEU.

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Library General Public
License as published by the Free Software Foundation; either
version 2 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty
of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU Library General Public License for more details.

You should have received a copy of the GNU Library General Public
License along with this library; see the file COPYING.Library.
If not, write to the Free Software Foundation, Inc.,
59 Temple Place, Suite 330, Boston, MA 02111-1307, USA.
*/

//	Hilfetextparameter

HelpFmt = string !
@name
\Name
@ident
\Synopsis
@synopsis
\Description
#if	Lang == "de"
Folgende Optionen und Argumente werden vom Kommando |$!| akzeptiert:
#else
The following options and arguments are accepted from command |$!|:
#endif
@arglist 12
\Environment
@environ 12
!;

str VersionFmt = string !
\hang
|$!| - $(Ident:%s),
Version $(Version:%s)
\br
$(Copyright:%s)
!;

PgmEnv("SHELL", "Shell=#1", string !
:*:determines shell for system calls.
:de:legt die Shell für Systemaufrufe fest.
!);
PgmEnv("PAGER", "Pager='|'+#1", string !
:*:determines command for crt viewing
:de:bestimmt das Kommando für den Seitenfilter.
!);
PgmEnv("MessageHandler", "MessageHandler=#1", string !
:*:determines command for displaying messages.
:de:definiert das Kommando zur Ausgabe von Fehlermeldungen.
!);

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
str help_man = "usage(NULL, \"|efeudoc --man -\")";
str help_lp = "usage(NULL, \"|efeudoc --lp -\")";

PgmOpt("?", "usage(UsageFmt, ioerr); exit(0)", NULL);
PgmOpt("-help", "eval(help_term); exit(0)", NULL);
PgmOpt("-help=type", "eval(psub(help_command)); exit(0)", string !
:*:
print usage of command. The optional parameter <type>
determines the formatting and output of the description.
[term]	display on terminal (default)
[raw]	raw data for |efeudoc|
[lp]	send to printer
:de:
gibt eine Beschreibung des Kommandos aus.
Der zusätzliche Parameter <type> bestimmt die Formatierung und
die Ausgabe der Beschreibung.
[term]	Terminalausgabe (default)
[raw]	Rohformat für |efeudoc|
[lp]	Ausgabe zum Drucker
!);

PgmOpt("-info", "Info(NULL, iostd); exit(0)", NULL);
PgmOpt("-info=entry", "Info(#1, iostd); exit(0)", string !
:*:show command information
:de:listet verfügbare Informationseinträge des Kommandos auf.
!);

PgmOpt("-dump", "InfoDump(NULL, iostd); exit(0)", NULL);
PgmOpt("-dump=entry", "InfoDump(#1, iostd); exit(0)", string !
:*:dump command information to standard output.
schreibt sämtliche Informationseinträge des Kommandos zur Standardausgabe.
!);

PgmOpt("-version", "usage(VersionFmt, ioerr); exit(0)", string !
:*:shows version information
:de:gibt die Versionsnummer des Kommandos aus.
!);

PgmOpt("-debug=mode", "setres(\"Debug\", #1)", string !
:*:sets debug level for command
:de:Setzt den Debug-Level für das Kommando.
Folgende Angaben sind möglich:
[none]	Keine Meldungen ausgeben;
[err]	Fehlermeldungen ausgeben;
[note]	Anmerkungen ausgeben;
[stat]	Verarbeitungsstatistiken ausgeben;
[trace]	Ablaufinformationen ausgeben;
[debug]	Debugginginformationen ausgeben (default);
!);

PgmOpt("-verbose", "setres(\"Debug\", \"stat\")", string !
:*:sets debug level to |stat|.
:de:Setzt den Debug-Level auf |stat|.
!);

