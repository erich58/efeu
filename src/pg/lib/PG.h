/*
:*:	interface to PostgreSQL
:de:	Interface zu PostgreSQL

$Header <DB/PG.h>
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

#ifndef	DB_PG_h
#define	DB_PG_h	1

#include <EFEU/refdata.h>
#include <EFEU/mdmat.h>
#include <EFEU/io.h>
#include <libpq-fe.h>

/*
:de:
Der Datentyp |$1| repräsentiert eine Verbindung zum Datenbankserver.
:*:
The structure |$1| represents a connection to a database server.
*/

typedef struct {
	REFVAR;		/* Referenzzähler */
	PGconn *conn;	/* Verbindungsdaten */
	PGresult *res;	/* Ergebnisdaten */
	FILE *trace;	/* Trace File */
	int lock;	/* Sperre für exec-Befehle */
} PG;

extern PG *PG_connect (const char *def);
extern int PG_print (IO *io, PG *pg);

extern void PG_clear (PG *pg);
extern int PG_exec (PG *pg, const char *cmd, ExecStatusType type);
extern const char *PG_status (PG *pg);
extern int PG_command (PG *pg, const char *cmd);

extern int PG_query (PG *pg, const char *cmd);
extern int PG_ntuples (PG *pg);
extern int PG_nfields (PG *pg);
extern char *PG_fname (PG *pg, int field);
extern char *PG_value (PG *pg, int tuple, int field);

extern IO *PG_open (PG *pg, const char *name, const char *mode);

extern mdmat *PG_mdmat (PG *pg, const EfiType *type,
	const char *value, const char *axis);

extern int PG_expandlim;
extern void SetupPG (void);
extern void _init (void);

/*
$SeeAlso
\mref{refdata(3)},
\mref{PG_connect(3)},
\mref{PG_exec(3)},
\mref{PG_open(3)},
\mref{PG_query(3)},
\mref{SetupPG(3)}.\br
PostgreSQL Programmer's Guide.
*/

#endif	/* DB/PG.h */
