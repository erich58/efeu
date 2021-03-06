/*
:de:	SQL-Abfrage von Tupeln
:*:	send a tuple query to backend

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

#define	M_NODATA	"no data available.\n"
#define	M_NF	"PG_head: field number $1 is out of range 0..$2.\n"
#define	M_NT	"PG_head: tuple number $1 is out of range 0..$2.\n"

/*
:*:
The function |$1| submits the query <cmd> to the database server with
expected return status PGRES_TUPLES_OK.
:de:
Die Funktion |$1| übermittelt die Datenbankanfrage <cmd> an den
Datenbankserver mit erwartetem Rückgabestatus PGRES_TUPLES_OK.

$Diagnostic
:*:
The function |$1| returns 1 on success and 0 on failure.
:de:
Die Funktion |$1| liefert 1 bei Erfolg und 0 bei Mißerfolg.
*/

int PG_query (PG *pg, const char *cmd)
{
	return PG_exec(pg, cmd, PGRES_TUPLES_OK);
}

/*
:de:
Die Funktion |$1| liefert die Zahl der Tupel der letzten Datenbankanfrage.
:*:
The function |$1| returns the number of tuples of last request.
*/

int PG_ntuples (PG *pg)
{
	return (pg && pg->res) ? PQntuples(pg->res) : 0;
}

/*
:de:
Die Funktion |$1| liefert die Zahl der Felder der letzten Datenbankanfrage.
:*:
The function |$1| returns the number of fields of last request.
*/

int PG_nfields (PG *pg)
{
	return (pg && pg->res) ? PQnfields(pg->res) : 0;
}

/*
:de:
Die Funktion |$1| liefert die Bezeichnung des Feldes <field>.
der letzten Datenbankanfrage.
:*:
The function |$1| returns the label of field <field> of last request.
*/

char *PG_fname (PG *pg, int field)
{
	int nfields = (pg && pg->res) ? PQnfields(pg->res) : 0;

	if	(field < 0 || field >= nfields)
	{
		PG_info(pg, M_NF, "dd", field, nfields);
		return NULL;
	}

	return PQfname(pg->res, field);
}

/*
:de:
Die Funktion |$1| liefert den Wert vom Feld <field> im Tupel <tuple>
der letzten Datenbankanfrage.
:*:
The function |$1| returns the value of field <field> in tuple <tuple>
of last request.
*/

char *PG_value (PG *pg, int tuple, int field)
{
	int ntuples = (pg && pg->res) ? PQntuples(pg->res) : 0;
	int nfields = (pg && pg->res) ? PQnfields(pg->res) : 0;

	if	(tuple < 0 || tuple >= ntuples)
	{
		PG_info(pg, M_NT, "dd", tuple, ntuples);
		return NULL;
	}

	if	(field < 0 || field >= nfields)
	{
		PG_info(pg, M_NF, "dd", field, nfields);
		return NULL;
	}

	return PQgetvalue(pg->res, tuple, field);
}

#endif

/*
$SeeAlso
\mref{PG_connect(3)},
\mref{PG_exec(3)},
\mref{PG_open(3)},
\mref{SetupPG(3)},
\mref{PG(7)}.\br
PostgreSQL Programmer's Guide.
*/
