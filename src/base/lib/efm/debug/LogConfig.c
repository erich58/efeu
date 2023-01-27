/*
:*:logging configuration
:de:Ausgabekonfiguration für Protokolle

$Copyright (C) 2009 Erich Frühstück
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

#include <EFEU/LogConfig.h>
#include <EFEU/procenv.h>
#include <EFEU/strbuf.h>
#include <EFEU/vecbuf.h>
#include <EFEU/mstring.h>
#include <stdio.h>
#include <ctype.h>

#define	LOGDEF_BSIZE	30
#define	ENTRY_BSIZE	60

static LogOutput *default_out = NULL;

#define	MASK_EXACT(level)	(1 << ((level) - 1))
#define	MASK_FULL(level)	(~(~0 << (level)))
#define	MASK_DEFAULT		MASK_FULL(LOGLEVEL_NOTE)

static STRBUF(sbuf, 0);
static int LogConfigChange = 1;
static int LogConfigLock = 0;

static int default_mask = MASK_DEFAULT;

typedef struct {
	char *select;	/* Auswahl */
	LogOutput *out;	/* Ausgabestruktur */
} LOGDEF;

static int log_match (LogControl *ctrl, char *select);

static VECBUF(vbuf, LOGDEF_BSIZE, sizeof(LOGDEF));

static unsigned set_mask (unsigned base, int n);
static void mask_name (const char *p, unsigned *base);
static unsigned mask_list (char *list);
static int match_name (const char *name, char *pattern);
static int match_list (const char *name, char *list);
static void sub_match (LogControl *ctrl, char *select, int *flag);

static int add_str (StrBuf *sb, const char *def, int delim);
static char *get_path (char *p);
static void def_clean (void *data);

static int pfx_cmd(int c)
{
	switch (c)
	{
	case ' ':
	case '\t':
	case '\n':
		break;
	case '@':
		vb_clear(&vbuf, def_clean);
		DelLogOutput(default_out);
		default_out = NULL;
		break;
	case '+':
		default_mask = set_mask(default_mask, 1);
		break;
	case '-':
		default_mask = set_mask(default_mask, -1);
		break;
	case ':':
		default_mask = MASK_DEFAULT;
		break;
	default:
		return 0;
	}

	return 1;
}

/*
Die Funktion |$1| erweitert/setzt die Protokolldefinitionen entsprechend
dem Definitionsstring <def>. Die Protokolldefinitionen liefern die Datenbasis
für die Protokollausgabe mithilfe einer Steuerstruktur vom Typ |LogControl|.
Sie werden intern in einer Tabelle verwaltet.

Falls das erste Zeichen des Definitionsstrings <def> ein
Klammeraffe |'@'| ist, wird die Tabelle vor der weitern Interpretation
des Definitionsstrings gelöscht. Ansonsten wird die Tabelle erweitert.

Der Definitionsstring besteht aus mehreren, durch weiße Zeichen getrennte
Protokolldefinitionen. Diese besteht aus den folgenden Komponenten:

*	Liste von Selektoren
*	Ausgabepfad (<path>)
*	Ausgabeflags (<flags>)

Die allgemeine Syntax ist:
	<sel>[|;|<sel>][|=|<path>]

Die einzelnen Komponenten sind optional. Die Ausgabeflags können an einer
beliebigen Stelle der Protokolldefinition stehen. Die Bedeutung der
Trennzeichen kann entweder mit einem Gegenschrägstrich, oder durch
einfaches Hochkomma aufgehoben werden.

Anstelle des Definitionsstrings kann auch eine folge von |+| und |-| stehen.
Damit wird der Vorgabewert für den Protokollierungslevel erhöht, bzw.
verringert.

Beim Ausgabepfad <path> handelt es sich um einen vordefinierten Standardnamen,
eine Datei oder einen Prozess, an dem die Daten weitergeleitet werden.

[stderr]
	Standardfehlerausgabe
[syslog]
	Syslog-Ausgabe zum Level |LOG_ERR|.
[syslog.<level>]
	Syslog-Ausgabe zum Level <level>
	(err, warning, notice, info, debug)
[<path>]
	Anhängen an Datei.

Die Selektoren bestimmen, für welche Steuerstrukturen die Protokolldefinition
gilt. Sie haben die Form <name>[|,|<name>]|.|<level>[|,|<level>]. Anstelle
von <name> kann auch ein Stern (|*|) für alle angegeben werden.
Fehlt der Name, gilt der Selektor für die Vorgabedefinition.

Für die Levels kann |none|, |err|, |warn|, |note|, |info|, |debug|, |trace| oder
|*| angegeben werden. Ein fehlender Level wird wie |*| behandelt. Ein Level
aktiviert auch alle anderen mit einer höheren Priorität, außer dem Namen
wird ein |=| vorangestellt. Die Selektion des Levels kann negiert werden,
indem ein |!| vorangestellt wird.
*/

void LogConfig (const char *def)
{
	char *p;

	if	(!def)	return;

	LogConfigLock++;

	if	(*def == 0)
	{
		vb_clear(&vbuf, def_clean);
		default_mask = MASK_DEFAULT;
	}

	while (pfx_cmd(*def))
		++def;

	while (*def)
	{
		while (isspace(*def))
			def++;

		sb_setpos(&sbuf, 0);

		while (*def && !isspace(*def))
		{
			if	(*def == '\\')
			{
				if	(strchr(" \t\n\\\'[]", def[1]))
					def++;

				sb_putc(*def++, &sbuf);
			}
			else if	(*def == '\'')
			{
				def += add_str(&sbuf, def, '\'');
			}
			else	sb_putc(*def++, &sbuf);
		}

		p = sb_nul(&sbuf);

		if	(*p == 0)
		{
			;
		}
		else if	(*p == '=')
		{
			DelLogOutput(default_out);
			default_out = NewLogOutput( p + 1);
		}
		else if	(*p == '.')
		{
			char *path = get_path(p);

			if	(path)
			{
				DelLogOutput(default_out);
				default_out = NewLogOutput(path);
			}

			default_mask = mask_list(p + 1);
		}
		else if	(*p == '~')
		{
			LOGDEF *def;
			size_t n;

			p++;
			get_path(p);

			for (n = 0, def = vbuf.data; n < vbuf.used; n++)
			{
				if	(strcmp(p, def[n].select) == 0)
				{
					def_clean(vb_delete(&vbuf, n, 1));
					break;
				}
			}
		}
		else
		{
			LOGDEF *def = vb_next(&vbuf);
			def->select = mstrcpy(p);
			def->out = NewLogOutput(get_path(def->select));
		}
	}

	LogConfigChange++;
	LogConfigLock--;
}

/*
Die Funktion |$1| prüft, ob sich die Kontrollstruktur <ctrl> auf dem Stand
der letzten Konfigurationsänderung befindet und erneuert bei Bedarf die
Protokolldefinitionen. Die Funktion wird implizit vor jeder Protokollausgabe
aufgerufen.
*/

int LogUpdate (LogControl *ctrl)
{
	if	(!ctrl)	return 0;

	if	(!LogConfigLock && ctrl->sync != LogConfigChange)
	{
		LOGDEF *def;
		size_t n;

		ctrl->enabled = 0;

		for (n = vbuf.used, def = vbuf.data; n--; def++)
			if	(log_match(ctrl, def->select))
				ctrl->enabled = 1;

		if	(MASK_EXACT(ctrl->level) & default_mask)
			ctrl->enabled = 1;

		ctrl->sync = LogConfigChange;
	}

	return ctrl->enabled;
}

/*
Logeinträge schreiben
*/

void log_write (LogControl *ctrl, const char *data, size_t pfx, size_t len)
{
	size_t n;
	int select = 0;
	LOGDEF *def;

	for (n = vbuf.used, def = vbuf.data; n--; def++)
	{
		if	(log_match(ctrl, def->select))
		{
			if	(def->out)
				def->out->write(def->out, data, pfx, len);
			else
				write(2, data + pfx, len - pfx);

			select = 1;
		}
	}

	if	(!select && (MASK_EXACT(ctrl->level) & default_mask))
	{
		if	(default_out)
			default_out->write(default_out, data, pfx, len);
		else
			write(2, data + pfx, len - pfx);
	}
}

static void def_clean (void *data)
{
	LOGDEF *def = data;
	DelLogOutput(def->out);
	memfree(def->select);
}

/*
Protokollmaske bestimmen
*/

static struct {
	char *name;
	unsigned flag;
} ptab[] = {
	{ "err", LOGLEVEL_ERR },
	{ "warn", LOGLEVEL_WARN },
	{ "note", LOGLEVEL_NOTE },
	{ "info", LOGLEVEL_INFO },
	{ "debug", LOGLEVEL_DEBUG },
	{ "trace", LOGLEVEL_TRACE },
};

static void mask_name (const char *p, unsigned *base)
{
	unsigned mask;
	int i;
	int neg;
	int exact;

	if	((neg = (*p == '!')))	p++;
	if	((exact = (*p == '=')))	p++;

	mask = 0;

	if	(strcmp(p, "*") == 0)
	{
		*base = neg ? 0 : MASK_FULL(LOGLEVEL_TRACE);
		return;
	}

	for (i = 0; i < tabsize(ptab); i++)
	{
		if	(strcmp(p, ptab[i].name) == 0)
		{
			if	(exact)
				mask = MASK_EXACT(ptab[i].flag);
			else	mask = MASK_FULL(ptab[i].flag);

			break;
		}
	}

	if	(neg)	*base &= ~mask;
	else		*base |= mask;
}

static unsigned mask_list (char *list)
{
	unsigned mask;
	char *p;

	mask = (*list == '!') ? ~0 : 0;

	while ((p = strchr(list, ',')))
	{
		*p = 0;
		mask_name(list, &mask);
		*p = ',';
		list = p + 1;
	}

	mask_name(list, &mask);
	return mask;
}

static unsigned set_mask (unsigned base, int n)
{
	int i;

	for (i = LOGLEVEL_TRACE; i >= LOGLEVEL_ERR; i--)
	{
		if	(MASK_EXACT(i) & base)
		{
			n += i;
			break;
		}
	}

	if	(n > LOGLEVEL_TRACE)	n = LOGLEVEL_TRACE;
	if	(n < LOGLEVEL_ERR)	n = LOGLEVEL_ERR;

	return MASK_FULL(n);
}

/*
Quotierten String anhängen
*/

static int add_str (StrBuf *sb, const char *def, int delim)
{
	int n;

	for (n = 1; def[n]; n++)
	{
		if	(def[n] == delim)
		{
			n++;
			break;
		}

		sb_putc(def[n], &sbuf);
	}

	return n;
}


/*
Pfadposition in Definitionszeile bstimmen
*/

static char *get_path (char *p)
{
	int last;

	for (last = 0; *p; last = *p++)
	{
		if	(*p == '=' && last != '.' && last != '!')
		{
			*p++ = 0;
			return p;
		}
	}

	return NULL;
}

/*
Test auf Definition in der Protokollkonfiguration
*/

static int match_name (const char *name, char *pattern)
{
	int neg;

	if	((neg = (*pattern == '!')))	pattern++;

	if	(pattern[0] == '*' && pattern[1] == 0)
		return 1;

	return (name && strcmp(name, pattern) == 0) ? !neg : neg;
}

static int match_list (const char *name, char *list)
{
	int flag = 0;
	char *p;

	while ((p = strchr(list, ',')))
	{
		*p = 0;
		flag |= match_name(name, list);
		*p = ',';
		list = p + 1;
	}

	flag |= match_name(name, list);
	return flag;
}

static void sub_match (LogControl *ctrl, char *select, int *flag)
{
	char *p;
	int match;
	int mask;
	
	if	((p = strchr(select, '.')))
	{
		*p = 0;
		match = match_list(ctrl->name, select);
		*p = '.';
		mask = mask_list(p + 1);
	}
	else
	{
		match = match_list(ctrl->name, select);
		mask = ~0;
	}

	if	(!match)	return;

	*flag = (MASK_EXACT(ctrl->level) & mask) ? 1 : 0;
}

static int log_match (LogControl *ctrl, char *select)
{
	char *p;
	int flag = 0;

	while ((p = strchr(select, ';')))
	{
		*p = 0;
		sub_match(ctrl, select, &flag);
		*p = ';';
		select = p + 1;
	}

	sub_match(ctrl, select, &flag);
	return flag;
}
