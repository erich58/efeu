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
#define	ERRM	"fatal error in %s: object (%p): bad magic number.\n"

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

	if	(!rd)
		return NULL;

#ifdef	REFDATA_MAGIC
	if	(rd->refmagic != REFDATA_MAGIC)
	{
		fprintf(stderr, ERRM, name, rd);
		abort();
		exit(EXIT_FAILURE);
	}
#endif

	if	(!rd->reftype)
		return NULL;

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

static void debug_id (IO *log, const RefData *rd, const char *cmd)
{
	int i;

	for (i = 0; i < debug_depth; i++)
		io_putc('\t', log);

	io_puts(cmd, log);
	io_putc('(', log);

	if	(rd->reftype->label)
	{
		io_puts(rd->reftype->label, log);
		io_putc(' ', log);
	}

	io_printf(log, "%lu %p)", (unsigned long) rd->refcount, rd);
}

static IO *debug_log (const RefData *rd)
{
	return LogOpen(rd->reftype->log);
}

static void std_debug (const RefData *rd, const char *cmd)
{
	IO *log = debug_log(rd);

	if	(log)
	{
		char *ident;
		
		debug_lock++;
		ident = rd_ident(rd);
		debug_id(log, rd, cmd);

		if	(ident)
		{
			io_putc(' ', log);
			io_puts(ident, log);
		}

		io_close(log);
		memfree(ident);
		debug_lock--;
	}
}

void rd_debug (const void *data, const char *fmt, ...)
{
	const RefData *rd = data;
	IO *log = rd ? debug_log(rd) : NULL;

	if	(log)
	{
		debug_id(log, rd, KEY_DEBUG);

		if	(fmt)
		{
			va_list args;

			va_start(args, fmt);
			io_putc(' ', log);
			io_vxprintf(log, fmt, args);
			va_end(args);
		}
			
		io_close(log);
	}
}

void rd_log (const void *data, const char *fmt, const char *def, ...)
{
	va_list list;

	va_start(list, def);
	rd_vlog(data, fmt, def, list);
	va_end(list);
}

void rd_vlog (const void *data, const char *fmt, const char *def, va_list list)
{
	const RefData *rd = data;

	if	(rd && rd->reftype->log)
	{
		char *id = msprintf("%s(%p %d)", 
			rd->reftype->label ? rd->reftype->label : "",
			rd->refcount);
		log_psubvarg(rd->reftype->log, fmt, id, def, list);
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
#ifdef	REFDATA_MAGIC
		rd->refmagic = REFDATA_MAGIC;
#endif
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

void *rd_deref (void *data)
{
	RefData *rd = rd_check(data, "rd_deref");

	if	(rd == NULL)	return rd;

	if	(rd->refcount > 1)
	{
		rd->refcount--;
		std_debug(rd, KEY_DEREF);
		return rd->refcount ? rd : NULL;
	}
	else
	{
		int indent = LogUpdate(rd->reftype->log);
		std_debug(rd, KEY_CLEAN);
		rd->refcount = 0;

		debug_depth += indent;

		if	(rd->reftype->clean)
			rd->reftype->clean(data);

		debug_depth -= indent;
		return NULL;
	}
}

/*	Referenzzähler verringern/Daten freigeben
*/

void rd_clean (void *data)
{
	rd_deref(data);
}
