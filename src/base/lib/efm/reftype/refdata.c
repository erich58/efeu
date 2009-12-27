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
#include <EFEU/io.h>
#include <EFEU/parsub.h>
#include <EFEU/Debug.h>

#define	ERR	"fatal error in %s: use of a deleted object (%lx).\n"

#define	KEY_ALLOC	"alloc"
#define	KEY_REFER	"refer"
#define	KEY_DEREF	"deref"
#define	KEY_CLEAN	"clean"
#define	KEY_DEBUG	"debug"

/*	Hilfsfunktion zur Überprüfung eines Referenzobjektes.
	Bei gültigen Daten wird der Pointer geliefert.
	Die Funktion entfernt gleichzeitig eine const-Qualifikation
*/

static refdata_t *rd_check (const void *data, const char *name)
{
	refdata_t *rd = (refdata_t *) data;

	if	(rd && rd->reftype)
	{
		if	(rd->refcount == 0)
		{
			fprintf(stderr, ERR, name, (ulong_t) rd);
			exit(EXIT_FAILURE);
			return NULL;
		}
		else	return rd;
	}
	else	return NULL;
}

/*	Debug - Information
*/

static int debug_depth = 0;

static void debug_id (io_t *log, const refdata_t *rd, const char *cmd)
{
	io_nputc('\t', log, debug_depth);
	io_printf(log, "%s(", cmd);

	if	(rd->reftype->label)
		io_printf(log, "%s ", rd->reftype->label);

	io_printf(log, "%#lx)", (ulong_t) rd);
}

static io_t *debug_log (const refdata_t *rd)
{
	reftype_t *type = (reftype_t *) rd->reftype;

	if	(type->sync < DebugChangeCount)
	{
		type->sync = DebugChangeCount;
		io_printf(type->log, "STOP debugging %s\n", type->label);
		rd_deref(type->log);
		type->log = rd_refer(LogOut(type->label, DBG_DEBUG));
		io_printf(type->log, "START debugging %s\n", type->label);
	}

	return type->log;
}

static void std_debug (const refdata_t *rd, const char *cmd)
{
	io_t *log = debug_log(rd);

	if	(log)
	{
		char *ident = rd_ident(rd);

		debug_id(log, rd, cmd);

		if	(ident)
		{
			io_printf(log, " %s", ident);
			memfree(ident);
		}

		io_putc('\n', log);
	}
}

void rd_debug (const void *data, const char *fmt, ...)
{
	const refdata_t *rd = data;
	io_t *log = rd ? debug_log(rd) : NULL;

	if	(log)
	{
		debug_id(log, rd, KEY_DEBUG);

		if	(fmt)
		{
			va_list args;

			va_start(args, fmt);
			io_putc(' ', log);
			io_vprintf(log, fmt, args);
			va_end(args);
		}
			
		io_putc('\n', log);
	}
}

/*	Identifikation generieren
*/

char *rd_ident (const void *data)
{
	refdata_t *rd = rd_check(data, "rd_ident");
	return (rd && rd->reftype->ident) ? rd->reftype->ident(rd) : NULL;
}


/*	Datenobjekt initialisieren
*/

void *rd_init (const reftype_t *type, void *data)
{
	refdata_t *rd = data;

	if	(rd)
	{
		rd->reftype = type;
		rd->refcount = 1;
		std_debug(rd, KEY_ALLOC);
	}

	return rd;
}


/*	Datenobjekt generieren
*/

void *rd_create (const reftype_t *type)
{
	if	(type && type->admin)
	{
		refdata_t *rd = type->admin(NULL, NULL);

		if	(rd)
		{
			rd->reftype = type;
			rd->refcount = 1;
			std_debug(rd, KEY_ALLOC);
		}

		return rd;
	}

	return NULL;
}


/*	Referenzzähler erhöhen
*/

void *rd_refer (const void *data)
{
	refdata_t *rd = rd_check(data, "rd_refer");

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
	refdata_t *rd = rd_check(data, "rd_deref");

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

		if	(rd->reftype == NULL)
			;
		else if	(rd->reftype->clean)
			rd->reftype->clean(data);
		else if	(rd->reftype->admin)
			rd->reftype->admin(data, NULL);

		debug_depth -= indent;
	}
}
