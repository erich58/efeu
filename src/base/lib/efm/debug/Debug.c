/*
:*:	debugmode
:de:	Debugmodus

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

#include <EFEU/Debug.h>
#include <EFEU/mstring.h>
#include <EFEU/Resource.h>
#include <EFEU/parsearg.h>
#include <EFEU/ftools.h>
#include <EFEU/vecbuf.h>
#include <EFEU/procenv.h>

/*
:de:
Die Variable |$1| wird bei jeder Änderung des Debug-Modus
um 1 erhöht. Damit können Funktionen testen, ob Log-Dateien
noch aktuell sind oder neu abgefragt werden müssen.
*/

int DebugChangeCount = 0;

typedef struct {
	char *name;
	FILE *file;
	IO *out;
	int level;
} LOGDEF;

static LOGDEF log_all = { NULL, NULL, NULL, DBG_NOTE };

static VECBUF(log_tab, 32, sizeof(LOGDEF));

static int log_cmp (const void *a, const void *b)
{
	return mstrcmp(((LOGDEF *) a)->name, ((LOGDEF *) b)->name);
}

static void log_clean (void *data)
{
	LOGDEF *log = data;
	io_close(log->out);
	fileclose(log->file);
	log->out = NULL;
	log->file = NULL;
}

static void cleanup (void *par)
{
	vb_clean(&log_tab, log_clean);
	log_clean(&log_all);
}

static void add_log (char *name, FILE *file, IO *out, int level)
{
	LOGDEF key, *log;

	if	(name == NULL || mstrcmp("all", name) == 0)
	{
		memfree(name);
		io_close(log_all.out);
		log_all.file = filerefer(file);
		log_all.out = rd_refer(out);
		log_all.level = level;
	}
	else
	{
		key.name = name;
		key.level = level;
		key.file = filerefer(file);
		key.out = rd_refer(out);
		log = vb_search(&log_tab, &key, log_cmp, VB_REPLACE);

		if	(log)
			log_clean(log);
	}
}

/*
:de:
Die Funktion |$1| setzt den Debuglevel für die Ausgabe von
Fehlermeldungen. Der Definitionsstring besteht aus einer
durch ein weißes Zeichen, Strichpunkt oder Dopelpunkt getrennten
Liste von Kennungen. Eine Kennung hat die allgemeine Gestalt
<namelist>[<log>]|=|<level>. Dabei gibt <namelist> eine durch Komma
getrennte Liste von Debugklassen an. Eine spezielle Klasse ist
|all|. Sie kommt immer dann zum Einsatz, wenn für eine
Debugklasse keine  spezielle Regel gefunden wurde.
\par
Die Debugklassen sind frei gewählte Namen bei den Debug-Aufrufen
der einzelnen Bibliotheksfunktionen. Ob und welche Debugklasse eine
Bibliotheksfunktion verwendet, kann am besten in den entsprechenden
Handbuch zu der Funktion, oder in den entsprechenden Sourcen
nachgeschlagen werden.
\par
Der optionale Parameter <log> definiert das Logfile für die
entsprechenden Klassen. Fehlt die Angabe, wird der Standardfehlerkanal
verwendet.
\par
Als Debuglevel kann einer der folgenden Kennungen angegeben werden:
|none|, |err|, |note|, |stat|, |trace| und |debug|. Die Level
sind hierarchisch, d.H. die Angabe von |trace| bewirkt automatisch
auch die Ausgabe aller Meldungen vom Type |err| und |note|.
Mit |none| werden sämtliche Fehlermeldungen unterdrückt.
\par
Eine Kennung <key> ohne Zuweisungszeichen ist äquivalent zu |all=|<key>.
*/

void DebugMode (const char *def)
{
	static int cleanup_registered = 0;
	char **list;
	size_t i, n;
	int level;

	if	(!cleanup_registered)
	{
	/*	Die Aufräumfunktion cleanup() muß nach der Aufräumfunktion
		für Dateien registriert werden. Der Aufruf fileident(NULL)
		erzwingt die Registrierung.
	*/
		fileident(NULL); 
		proc_clean(cleanup, NULL);
		cleanup_registered = 1;
	}

	if	(def == NULL)	return;

	n = mstrsplit(def, " \t\n;:", &list);

	for (i = 0; i < n; i++)
	{
		AssignArg *x;
		char *name, *ptr;
		FILE *file;
		IO *out;

		if	(strchr(list[i], '=') == NULL)
		{
			log_all.level = DebugKey(list[i]);
			continue;
		}

		x = assignarg(list[i], NULL, ";:");
		file = NULL;
		out = NULL;

		if	(x->opt)
		{
			if	(x->opt[0] == '+')
			{
				x->opt++;
				file = fileopen(x->opt, "az");
				fprintf(file, "----\n");
			}
			else	file = fileopen(x->opt, "wz");

			out = io_stream(x->opt, file, fileclose);
		}

		level = DebugKey(x->arg);
		ptr = x->name;

		do
		{
			name = mstrcut(ptr, &ptr, ",", 1);
			add_log(name, file, out, level);
		}
		while (ptr != NULL);

		rd_deref(out);
		memfree(x);
	}

	memfree(list);
	DebugChangeCount++;
}


static LOGDEF *LogDef (const char *class, int level)
{
	static int need_setup = 1;
	LOGDEF *log;

	if	(need_setup)
	{
		DebugMode(NULL);
		need_setup = 0;
	}

	log = &log_all;

	if	(class && log_tab.used)
	{
		LOGDEF key;
		key.name = (char *) class;
		log = vb_search(&log_tab, &key, log_cmp, VB_SEARCH);

		if	(log == NULL)	log = &log_all;
	}

	if	(level >= DBG_DIM)	level = DBG_DIM - 1;
	if	(level > log->level)	return NULL;

	return log;
}

static LOGDEF *ParseLogDef (const char *def)
{
	char *p, *class;
	LOGDEF *log;

	if	(def == NULL)	return NULL;

	p = strchr(def, '=');

	if	(p)
	{
		class = mstrncpy(def, p - def);
		log = LogDef(class, DebugKey(p + 1));
		memfree(class);
	}
	else	log = LogDef(NULL, DebugKey(def));

	return log;
}

/*
:de:
Die Funktion |$1| ruft die Logdatei zu einer Debugklasse <class> mit
Debuglevel <level> ab. Falls keine Meldungen auszugeben sind,
liefert die Funktion einen Nullpointer.
*/

FILE *LogFile (const char *class, int level)
{
	LOGDEF *log = LogDef(class, level);
	return log ? (log->file ? log->file : stderr) : NULL;
}

/*
:de:
Die Funktion |$1| ist äquivalent zu |LogFile|, jedoch wird anstelle
einer FILE-Struktur die zugehörige IO-Struktur geliefert.
*/

IO *LogOut (const char *class, int level)
{
	LOGDEF *log = LogDef(class, level);
	return log ? (log->out ? log->out : ioerr) : NULL;
}

/*
:de:
Die Funktion |$1| ermittelt aus einem String <def> der Form
<"<class>|=|<level>"> die Debugklasse <class> und den Debuglevel <level>
und ruft die zugehörige Logdatei ab. Die beiden Funktionsaufrufe
|ParseLogFile("db=stat")| und |LogFile("db", DBG_STAT)| sind gleichwertig.
*/

FILE *ParseLogFile (const char *def)
{
	LOGDEF *log = ParseLogDef(def);
	return log ? (log->file ? log->file : stderr) : NULL;
}

/*
:de:
Die Funktion |$1| ist äquivalent zu |ParseLogFile|, jedoch wird anstelle
einer FILE-Struktur die zugehörige IO-Struktur geliefert.
*/

IO *ParseLogOut (const char *def)
{
	LOGDEF *log = ParseLogDef(def);
	return log ? (log->out ? log->out : ioerr) : NULL;
}

/*
$SeeAlso
\mref{DebugKey(3)},
\mref{Message(3)},
\mref{Debug(7)}.
*/

