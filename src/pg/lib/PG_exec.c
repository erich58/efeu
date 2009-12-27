/*
:de:	SQL-Abfragefunktionen
:*:	query execution functions

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
#include <EFEU/mstring.h>
#include <EFEU/strbuf.h>
#include <EFEU/Debug.h>

#if	HAS_PQ

#define	M_LOCK	"$!: exec is locked.\n"
#define	M_NOMEM	"$!: not enough memmory to create data.\n"
#define	M_EXEC	"PQexec() failed.\n$1"
#define	M_STAT	"PQexec() returns $1 where $2 is expected.\n"
#define	M_CMD	"Command was: \"$1\".\n"

/*
:de:
Die Funktion |$1| übermittelt die Datenbankanfrage <cmd> an den
Datenbankserver mit erwarteten Rückgabestatus <stat>. Ein Statuswert
von |PGRES_EMPTY_QUERY| akzeptiert jeden Rückgabestatus außer Fehler.
:*:
The function|$1| submits the query <cmd> to the database server with
respected return status <stat>.
A value of |PGRES_EMPTY_QUERY| accepts any return status except errors.
*/

int PG_exec (PG *pg, const char *cmd, ExecStatusType stat)
{
	ExecStatusType res;

	if	(pg == NULL)	return 0;

	if	(pg->lock)
	{
		PG_info(pg, M_LOCK, NULL);
		return 0;
	}

	PG_clear(pg);

	if	(pg == NULL || cmd == NULL)
		return 0;

	rd_debug(pg, "PQexec(\"%s\")", cmd);
	pg->res = PQexec(pg->conn, cmd);

	if	(pg->res == NULL)
	{
		PG_info(pg, M_NOMEM, NULL);
		return 0;
	}

	res = PQresultStatus(pg->res);

	switch (res)
	{
	case PGRES_BAD_RESPONSE:
	case PGRES_NONFATAL_ERROR:
	case PGRES_FATAL_ERROR:
		PG_info(pg, M_EXEC, "s", PQresultErrorMessage(pg->res));
		PG_info(pg, M_CMD, "s", cmd);
		PG_clear(pg);
		return 0;
		break;
	default:
		break;
	}
	
	if	(stat != PGRES_EMPTY_QUERY && res != stat)
	{
		PG_info(pg, M_STAT,
			"ss", PQresStatus(res), PQresStatus(stat));
		PG_info(pg, M_CMD, "s", cmd);
		PG_clear(pg);
		return 0;
	}

	return 1;
}

/*
:de:
Die Funktion |$1| übermittelt die Datenbankanfrage <cmd> an den
Datenbankserver mit erwarteten Rückgabestatus PGRES_COMMAND_OK.
:*:
The function|$1| submits the query <cmd> to the database server with
respected return status PGRES_COMMAND_OK.
*/

int PG_command (PG *pg, const char *cmd)
{
	return PG_exec(pg, cmd, PGRES_COMMAND_OK);
}

void PG_serialize (PG *pg)
{
	if	(!pg || pg->trans)	return;

	PG_command(pg, "begin");
	PG_command(pg, "set transaction isolation level serializable");
	pg->trans = 1;
}

/*
:de:
Die Funktion |$1| löscht die Daten zur letzten SQL-Abfrage.
:*:
The function |$1| clears the data of the last query.
*/

void PG_clear (PG *pg)
{
	if	(pg && pg->res && !pg->lock)
	{
		PQclear(pg->res);
		pg->res = NULL;
	}
}

/*
:de:
Die Funktion |$1| liefert den Status der letzten Anfrage als Zeichenkette.
:*:
The Funktion |$1| returns the status of the last query as string.
*/

const char *PG_status (PG *pg)
{
	return (pg && pg->res) ? PQresStatus(PQresultStatus(pg->res)) : NULL;
}

#endif

/*
$Diagnostic
:de:
Die Funktionen |PG_exec| und |PG_command| liefern 1 bei Erfolg
und 0 bei Mißerfolg.
:*:
The functions |PG_exec| and |PG_command| returns 1 on success
and 0 on failure.

$SeeAlso
\mref{PG_connect(3)},
\mref{PG_open(3)},
\mref{PG_query(3)},
\mref{SetupPG(3)},
\mref{PG(7)}.\br
PostgreSQL Programmer's Guide.
*/
