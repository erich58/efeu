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

#define	M_NOMEM	"$!: not enough memory to create data.\n"
#define	M_BAD	"$!: connection to database server failed.\n"

static PG_t *pg_alloc (void)
{
	return memalloc(sizeof(PG_t));
}

static void pg_clean (PG_t *pg)
{
	if	(pg->res)
		PQclear(pg->res);

	if	(pg->trace)
	{
		PQuntrace(pg->conn);
		fileclose(pg->trace);
	}

	if	(pg->conn)
		PQfinish(pg->conn);

	memfree(pg);
}

static int conn_val (io_t *io, const char *name, const char *val)
{
	if	(val)
	{
		return io_printf(io, "\t%s = \"%s\"\n", name, val);
	}
	else	return io_printf(io, "\t%s = NULL\n", name);
}


static int conn_info (io_t *io, PGconn *conn)
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

int PG_print (io_t *io, PG_t *pg)
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

static char *pg_ident (PG_t *pg)
{
	strbuf_t *sb = new_strbuf(0);
	io_t *io = io_strbuf(sb);
	conn_info(io, pg->conn);
	io_close(io);
	return sb2str(sb);
}

static REFTYPE(pg_reftype, "PG", pg_ident, pg_clean);


/*
:de:
Die Funktion |$1| erzeugt eine Verbindung zu einem PostgreSQL Datenbankserver
und liefert eine Struktur vom Type |$1_t|.
:*:
The function |$1| makes a new connection to a PostgreSQL backend
and returns a |$1_t| structure.

$Diagnostic
:de:
Konnte keine Verbindung hergestellt werden, wird eine Fehlermeldung
ausgegeben und die Funktion |$1| liefert einen NULL-Pointer.
:*:
If the connection fails, an error message is displayed and
the function |$1| returns a NULL-pointer.
*/

PG_t *PG (const char *def)
{
	PGconn *conn;
	
	if	(def == NULL)	return NULL;

	conn = PQconnectdb(def);

	if	(conn == NULL)
	{
		Message("PG", DBG_ERR, M_NOMEM, NULL);
		return NULL;
	}
	else if	(PQstatus(conn) == CONNECTION_BAD)
	{
		Message("PG", DBG_ERR, M_BAD, ArgList("nc", def));
		conn_info(LogOut("PG", DBG_DEBUG), conn);
		PQfinish(conn);
		return NULL;
	}
	else
	{
		PG_t *pg = pg_alloc();
		pg->conn = conn;
		pg->res = NULL;
		pg->lock = 0;
		pg->trace = filerefer(LogFile("PG", DBG_TRACE));

		if	(pg->trace)
			PQtrace(pg->conn, pg->trace);

		return rd_init(&pg_reftype, pg);
	}
}

/*
$SeeAlso
refdata(3),
PG_exec(3),
PG_open(3),
PG_query(3),
SetupPG(3),
PG(7).\br
PostgreSQL Programmer's Guide.
*/
