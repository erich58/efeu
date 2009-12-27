/*
Applikationsdateien

$Copyright (C) 2001 Erich Frühstück
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

#include <EFEU/procenv.h>
#include <EFEU/mstring.h>
#include <EFEU/ftools.h>
#include <EFEU/appl.h>

#define	_String(x)	#x
#define	String(x)	_String(x)
#define	TOP		String(EFEUROOT)

/*
Die Variable |$1| bestimmt den Suchpfad für Applikationsdateien.
*/

char *ApplPath = NULL;

/*
Die Funktion |$1| initialisiert den Suchpfad |ApplPath| für Applikationsdateien.
Falls für <path> ein Nullpointer angegeben wurde und der Suchpfad bereits
initialisiert wurde, wird er von |$1| nicht verändert.
*/

void SetApplPath (const char *path)
{
	StrBuf *sb;
	char *p;

	if	(ApplPath && !path)	return;

	sb = sb_create(0);

/*	Basispfade
*/
	sb_putc('.', sb);

	if	(path)	sb_putc(':', sb), sb_puts(path, sb);

/*	Environment
*/
	if	((p = getenv("APPLPATH")) != NULL)
		sb_putc(':', sb), sb_puts(p, sb);

/*	Systemsuchpfade
*/
	sb_printf(sb, ":%s/lib/efeu/%%L/%%S", TOP);

	if	((p = getenv("HOME")) != NULL)
		sb_printf(sb, ":%s/lib/efeu/%%L/%%S", p);

	sb_puts(":./%L/%S", sb);
	memfree(ApplPath);
	ApplPath = sb2str(sb);
}


/*
Die Funktion |$1| sucht eine Applikationsdatei vom Type |type|
und öffnet sie als IO-Struktur. Der Type bestimmt das Teilverzeichnis
und den Filezusatz.

Folgende Typen sind in |<EFEU/appl.h>| vordefiniert:

[APPL_CNF]	Konfigurationsdate <name>|.cnf| unter |config|
[APPL_MSG]	Formatdatei <name>|.msg| unter |messages|
[APPL_HLP]	Kommandobeschreibung <name>|.hlp| unter |help|

Im Suchpfad wird %S durch das Teilverzeichnis ersetzt.
$Diagnostic
Falls die Datei nicht gefunden werden konnte,
liefert die Funktion |$1| einen Nullpointer.
*/

IO *io_applfile (const char *name, int type)
{
	char *p, *pfx, *sfx;
	IO *io;

	if	(name == NULL)	return NULL;

	switch (type)
	{
	case APPL_CNF:	pfx = "config"; sfx = "cnf"; break;
	case APPL_MSG:	pfx = "messages"; sfx = "msg"; break;
	case APPL_HLP:	pfx = "help"; sfx = "hlp"; break;
	default:	pfx = NULL; sfx = NULL; break;
	}

	p = fsearch(ApplPath, pfx, name, sfx);
	io = p ? io_fileopen(p, "r") : NULL;
	memfree(p);
	return io;
}
