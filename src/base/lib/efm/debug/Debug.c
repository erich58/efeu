/*
:*:	debugmode
:de:	Debugmodus

$Copyright (C) 2001 Erich Fr�hst�ck
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
#include <EFEU/KeyTab.h>

typedef struct {
	char *name;
	FILE *file;
	io_t *out;
	int level;
} LOGDEF;

static LOGDEF log_all = { NULL, NULL, NULL, DBG_NOTE };

static VECBUF(log_tab, 32, sizeof(LOGDEF));

static int log_cmp (const void *a, const void *b)
{
	return mstrcmp(((LOGDEF *) a)->name, ((LOGDEF *) b)->name);
}

static void log_clean (LOGDEF *log)
{
	io_close(log->out);
	fileclose(log->file);
	log->out = NULL;
	log->file = NULL;
}

static void cleanup (void)
{
	vb_clean(&log_tab, (clean_t) log_clean);
	log_clean(&log_all);
}

static void add_log (char *name, FILE *file, io_t *out, int level)
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
Die Funktion |$1| setzt den Debuglevel f�r die Ausgabe von
Fehlermeldungen. Der Definitionsstring besteht aus einer
durch ein wei�es Zeichen, Strichpunkt oder Dopelpunkt getrennten
Liste von Kennungen. Eine Kennung hat die allgemeine Gestalt
<namelist>[<log>]|=|<level>. Dabei gibt <namelist> eine durch Komma
getrennte Liste von Debugklassen an. Eine spezielle Klasse ist
|all|. Sie kommt immer dann zum Einsatz, wenn f�r eine
Debugklasse keine  spezielle Regel gefunden wurde.
\par
Die Debugklassen sind frei gew�hlte Namen bei den Debug-Aufrufen
der einzelnen Bibliotheksfunktionen. Ob und welche Debugklasse eine
Bibliotheksfubnktion verwendet, kann am besten in den entsprechenden
Handbuch zu der Funktion, oder in den entsprechenden Sourcen
nachgeschlagen werden.
\par
Der optionale Parameter <log> definiert das Logfile f�r die
entsprechenden Klassen. Fehlt die Angabe, wird der Standardfehlerkanal
verwendet.
\par
Als Debuglevel kann einer der folgenden Kennungen angegeben werden:
|none|, |err|, |note|, |stat|, |trace| und |debug|. Die Level
sind hierarchisch, d.H. die Angabe von |trace| bewirkt automatisch
auch die Ausgabe aller Meldungen vom Type |err| und |note|.
Mit |none| werden s�mtliche Fehlermeldungen unterdr�ckt.
\par
Eine Kennung <key> ohne Zuweisungszeichen ist �quivalent zu |all=|<key>.

$ERROR
:*:
A great number of messages is currently not controlled by the debugmode.
The adaptation is in progress.
:de:
Die Steuerung der Fehlermeldungen ist neu. Eine gro�e Zahl an Meldungen
wird derzeit nicht �ber den Debugmodus kontrolliert. Die Umstellung
ist aber im gang.
*/

void DebugMode (const char *def)
{
	static int cleanup_registered = 0;
	char **list;
	size_t i, n;
	int level;

	if	(!cleanup_registered)
	{
	/*	Die Aufr�umfunktion setup() mu� nach der Aufr�umfunktion
		f�r Dateien registriert werden. Der Aufruf fileident(NULL)
		erzwingt die Registrierung.
	*/
		fileident(NULL); 
		atexit(cleanup);
		cleanup_registered = 1;
	}

	n = strsplit(def, " \t\n;:", &list);

	for (i = 0; i < n; i++)
	{
		assignarg_t *x;
		char *name, *ptr;
		FILE *file;
		io_t *out;

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
			file = fileopen(x->opt, "az");
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

	if	(log_tab.used)
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
Die Funktion |$1| ist �quivalent zu |LogFile|, jedoch wird anstelle
einer FILE-Struktur die zugeh�rige IO-Struktur geliefert.
*/

io_t *LogOut (const char *class, int level)
{
	LOGDEF *log = LogDef(class, level);
	return log ? (log->out ? log->out : ioerr) : NULL;
}

/*
:de:
Die Funktion |$1| ermittelt aus einem String <def> der Form
<"<class>|=|<level>"> die Debugklasse <class> und den Debuglevel <level>
und ruft die zugeh�rige Logdatei ab. Die beiden Funktionsaufrufe
|ParseLogFile("db=stat")| und |LogFile("db", DBG_STAT)| sind gleichwertig.
*/

FILE *ParseLogFile (const char *def)
{
	LOGDEF *log = ParseLogDef(def);
	return log ? (log->file ? log->file : stderr) : NULL;
}

/*
:de:
Die Funktion |$1| ist �quivalent zu |ParseLogFile|, jedoch wird anstelle
einer FILE-Struktur die zugeh�rige IO-Struktur geliefert.
*/

io_t *ParseLogOut (const char *def)
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

