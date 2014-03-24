/*
:*:	database connection type
:de:	Datenbankverbindungstype

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

#include <DB/PG.h>
#include <EFEU/memalloc.h>
#include <EFEU/strbuf.h>
#include <EFEU/ftools.h>
#include <EFEU/Debug.h>

#if	HAS_PQ

#define	M_NOMEM	"$!: not enough memory to create data.\n"
#define	M_BAD	"$!: connection `$1' to database server failed.\n"

static void pg_clean (void *ptr)
{
	PG *pg = ptr;

	if	(pg->trans)
		PG_command(pg, "commit");

	if	(pg->res)
		PQclear(pg->res);

	if	(pg->trace)
	{
		PQuntrace(pg->conn);
		fileclose(pg->trace);
	}

	PG_info(pg, "$0: FINISH", NULL);

	if	(pg->conn)
		PQfinish(pg->conn);

	memfree(pg);
}

static int conn_val (IO *io, const char *name, const char *val)
{
	if	(val)
	{
		return io_xprintf(io, "\t%s = \"%s\"\n", name, val);
	}
	else	return io_xprintf(io, "\t%s = NULL\n", name);
}


static int conn_info (IO *io, PGconn *conn)
{
	int n;

	if	(PQstatus(conn) == CONNECTION_OK)
		n = io_puts("\tstatus = ok\n", io);
	else	n = io_puts("\tstatus = bad\n", io);

	n += conn_val(io, "dbname", PQdb(conn));
	n += conn_val(io, "user", PQuser(conn));
	n += conn_val(io, "pass", PQpass(conn));
	n += conn_val(io, "host", PQhost(conn));
	n += conn_val(io, "port", PQport(conn));
	n += conn_val(io, "tty", PQtty(conn));
	n += conn_val(io, "options", PQoptions(conn));
	return n;
}

int PG_print (IO *io, PG *pg)
{
	if	(pg && pg->conn)
	{
		int n = io_puts("PG = {\n", io);
		n += conn_info(io, pg->conn);
		n += io_puts("}", io);
		return n;
	}
	else	return io_puts("NULL", io);
}

static char *pg_ident (const void *ptr)
{
	const PG *pg = ptr;
	return msprintf("postgres[%s]", PQdb(pg->conn));
}

static RefType pg_reftype = REFTYPE_INIT("PG", pg_ident, pg_clean);

static LogControl pg_info = LOG_CONTROL("PG", LOGLEVEL_INFO);
static LogControl pg_trace = LOG_CONTROL("PG", LOGLEVEL_TRACE);
static LogControl pg_debug = LOG_CONTROL("PG", LOGLEVEL_DEBUG);

/*
:de:
Die Funktion |$1| erzeugt eine Verbindung zu einem PostgreSQL Datenbankserver
und liefert eine Struktur vom Type |PG|. Die Verbindung wird mit der Funktion
|PQconnectdb| aufgebaut, der Parameter <def> wird an diese Funktion
weitergereicht. Falls nur der Datenbankname angegeben wird, kann das
Schlüsselwort |dbname| entfallen.
:*:
The function |$1| makes a new connection to a PostgreSQL backend
and returns a |PG| structure.

$Diagnostic
:de:
Konnte keine Verbindung hergestellt werden, wird eine Fehlermeldung
ausgegeben und die Funktion |$1| liefert einen NULL-Pointer.
:*:
If the connection fails, an error message is displayed and
the function |$1| returns a NULL-pointer.
*/

PG *PG_connect (const char *def)
{
	PGconn *conn;
	
	if	(def == NULL)
	{
		return NULL;
	}
	else if	(*def == 0)
	{
		conn = PQconnectdb("");
	}
	else if	(strchr(def, '=') != NULL)
	{
		conn = PQconnectdb(def);
	}
	else
	{
		char *p = mstrpaste("=", "dbname", def);
		conn = PQconnectdb(p);
		memfree(p);
	}

	if	(conn == NULL)
	{
		PG_info(NULL, M_NOMEM, NULL);
		return NULL;
	}
	else if	(PQstatus(conn) == CONNECTION_BAD)
	{
		IO *io;
		PG_info(NULL, M_BAD, "s", def);

		if	((io = LogOpen(&pg_debug)))
		{
			conn_info(io, conn);
			io_close(io);
		}

		PQfinish(conn);
		return NULL;
	}
	else
	{
		PG *pg = memalloc(sizeof *pg);
		pg->conn = conn;
		pg->res = NULL;
		pg->lock = 0;
		pg->trans = 0;

		rd_init(&pg_reftype, pg);
		PG_info(pg, "$0: CONNECT", NULL);
		pg->trace = log_file(&pg_trace);

		if	(pg->trace)
			PQtrace(pg->conn, pg->trace);

		return pg;
	}
}

/*
:*:The function |$1| displays a report to error class |PG| and level
|DBG_INFO|.
:de:Die Funktion |$1| gibt eine Meldung zur Fehlerklasse |PG| vom Level
|DBG_INFO| aus.
*/

void PG_info (PG *pg, const char *fmt, const char *argdef, ...)
{
	IO *io = LogOpen(&pg_info);

	if	(io && fmt)
	{
		va_list list;
		ArgList *args;
		
		args = arg_create();

		if	(pg)
			arg_printf(args, "postgres[%s]", PQdb(pg->conn));
		else	arg_cadd(args, NULL);

		va_start(list, argdef);
		arg_append(args, argdef, list);
		va_end(list);
		io_psub(io, fmt, args);
		io_close(io);
		rd_deref(args);
	}
}

#endif

/*
$SeeAlso
\mref{refdata(3)},
\mref{PG_exec(3)},
\mref{PG_open(3)},
\mref{PG_query(3)},
\mref{SetupPG(3)},
\mref{PG(7)}.\br
PostgreSQL Programmer's Guide.
*/
