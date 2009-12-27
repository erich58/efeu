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

int PG_exec (PG_t *pg, const char *cmd, ExecStatusType stat)
{
	ExecStatusType res;

	if	(pg == NULL)	return 0;

	if	(pg->lock)
	{
		Message("PG", DBG_ERR, M_LOCK, NULL);
		return 0;
	}

	PG_clear(pg);

	if	(pg == NULL || cmd == NULL)
		return 0;

	pg->res = PQexec(pg->conn, cmd);

	if	(pg->res == NULL)
	{
		Message("PG", DBG_ERR, M_NOMEM, NULL);
		return 0;
	}

	res = PQresultStatus(pg->res);

	switch (res)
	{
	case PGRES_BAD_RESPONSE:
	case PGRES_NONFATAL_ERROR:
	case PGRES_FATAL_ERROR:
		Message("PG", DBG_ERR, M_EXEC,
			ArgList("nc", PQresultErrorMessage(pg->res)));
		Message("PG", DBG_NOTE, M_CMD,
			ArgList("nc", cmd));
		PG_clear(pg);
		return 0;
		break;
	default:
		break;
	}
	
	if	(stat != PGRES_EMPTY_QUERY && res != stat)
	{
		Message("PG", DBG_ERR, M_STAT,
			ArgList("ncc", PQresStatus(res), PQresStatus(stat)));
		Message("PG", DBG_NOTE, M_CMD,
			ArgList("nc", cmd));
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

int PG_command (PG_t *pg, const char *cmd)
{
	return PG_exec(pg, cmd, PGRES_COMMAND_OK);
}

/*
:de:
Die Funktion |$1| löscht die Daten zur letzten SQL-Abfrage.
:*:
The function |$1| clears the data of the last query.
*/

void PG_clear (PG_t *pg)
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

const char *PG_status (PG_t *pg)
{
	return (pg && pg->res) ? PQresStatus(PQresultStatus(pg->res)) : NULL;
}

/*
$Diagnostic
:de:
Die Funktionen |PG_exec| und |PG_command| liefern 1 bei Erfolg
und 0 bei Mißerfolg.
:*:
The functions |PG_exec| and |PG_command| returns 1 on success
and 0 on failure.

$SeeAlso
PG(3),
PG_open(3),
PG_query(3),
SetupPG(3),
PG(7).\br
PostgreSQL Programmer's Guide.
*/
