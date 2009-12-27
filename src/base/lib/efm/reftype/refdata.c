/*
Datenstrukturen mit Referenzzähler

$Copyright (C) 1995 Erich Frühstück
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


#include <EFEU/memalloc.h>
#include <EFEU/refdata.h>
#include <EFEU/ftools.h>
#include <EFEU/parsub.h>
#include <EFEU/Debug.h>
#include <assert.h>

#define	ERR	"fatal error in %s: use of a deleted object (%p).\n"
#define	ERR2	"fatal error in %s: object (%p): undefined reference type.\n"

#define	KEY_ALLOC	"alloc"
#define	KEY_REFER	"refer"
#define	KEY_DEREF	"deref"
#define	KEY_CLEAN	"clean"
#define	KEY_DEBUG	"debug"

/*	Hilfsfunktion zur Überprüfung eines Referenzobjektes.
	Bei gültigen Daten wird der Pointer geliefert.
	Die Funktion entfernt gleichzeitig eine const-Qualifikation
*/

static RefData *rd_check (const void *data, const char *name)
{
	RefData *rd = (RefData *) data;

	if	(rd == NULL)
		return NULL;

	if	(rd->reftype == NULL)
	{
/*
		fprintf(stderr, ERR2, name, rd);
		abort();
		exit(EXIT_FAILURE);
*/
		return NULL;
	}


	if	(rd->refcount == 0)
	{
		fprintf(stderr, ERR, name, rd);
		exit(EXIT_FAILURE);
		return NULL;
	}

	return rd;
}

/*	Debug - Information
*/

static int debug_depth = 0;
static int debug_lock = 0;

static void debug_id (FILE *log, const RefData *rd, const char *cmd)
{
	int i;

	for (i = 0; i < debug_depth; i++)
		putc('\t', log);

	fputs(cmd, log);
	putc('(', log);

	if	(rd->reftype->label)
	{
		fputs(rd->reftype->label, log);
		putc(' ', log);
	}

	fprintf(log, "%d %p)", rd->refcount, rd);
}

static FILE *debug_log (const RefData *rd)
{
	RefType *type = (RefType *) rd->reftype;

	if	(debug_lock)
		return NULL;

	if	(type->sync < DebugChangeCount)
	{
		type->sync = DebugChangeCount;

		if	(type->log)
		{
			fprintf(type->log, "STOP debugging %s\n", type->label);
			fileclose(type->log);
		}

		type->log = filerefer(LogFile(type->label, DBG_DEBUG));

		if	(type->log)
			fprintf(type->log, "START debugging %s\n", type->label);
	}

	return type->log;
}

static void std_debug (const RefData *rd, const char *cmd)
{
	FILE *log = debug_log(rd);

	if	(log)
	{
		char *ident;
		
		debug_lock++;
		ident = rd_ident(rd);
		debug_id(log, rd, cmd);

		if	(ident)
		{
			putc(' ', log);
			fputs(ident, log);
		}

		putc('\n', log);
		memfree(ident);
		debug_lock--;
	}
}

void rd_debug (const void *data, const char *fmt, ...)
{
	const RefData *rd = data;
	FILE *log = rd ? debug_log(rd) : NULL;

	if	(log)
	{
		debug_id(log, rd, KEY_DEBUG);

		if	(fmt)
		{
			va_list args;

			va_start(args, fmt);
			putc(' ', log);
			vfprintf(log, fmt, args);
			va_end(args);
		}
			
		putc('\n', log);
	}
}

/*	Identifikation generieren
*/

char *rd_ident (const void *data)
{
	RefData *rd = rd_check(data, "rd_ident");
	return (rd && rd->reftype->ident) ? rd->reftype->ident(rd) : NULL;
}


/*	Datenobjekt initialisieren
*/

void *rd_init (const RefType *type, void *data)
{
	RefData *rd = data;

	if	(rd)
	{
		rd->reftype = type;
		rd->refcount = 1;
		std_debug(rd, KEY_ALLOC);
	}

	return rd;
}


/*	Referenzzähler erhöhen
*/

void *rd_refer (const void *data)
{
	RefData *rd = rd_check(data, "rd_refer");

	if	(rd != NULL)
	{
		std_debug(rd, KEY_REFER);
		rd->refcount++;
	}

	return rd;
}

/*	Referenzzähler verringern/Daten freigeben
*/

void rd_deref (void *data)
{
	RefData *rd = rd_check(data, "rd_deref");

	if	(rd == NULL)	return;

	if	(rd->refcount > 1)
	{
		rd->refcount--;
		std_debug(rd, KEY_DEREF);
	}
	else
	{
		int indent = rd->reftype->log ? 1 : 0;

		std_debug(rd, KEY_CLEAN);
		rd->refcount = 0;

		debug_depth += indent;

		if	(rd->reftype->clean)
			rd->reftype->clean(data);

		debug_depth -= indent;
	}
}
