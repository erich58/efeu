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

#ifndef	_DB_PG_h
#define	_DB_PG_h	1

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
} PG_t;

extern PG_t *PG (const char *def);
extern int PG_print (io_t *io, PG_t *pg);

extern void PG_clear (PG_t *pg);
extern int PG_exec (PG_t *pg, const char *cmd, ExecStatusType type);
extern const char *PG_status (PG_t *pg);
extern int PG_command (PG_t *pg, const char *cmd);

extern int PG_query (PG_t *pg, const char *cmd);
extern int PG_ntuples (PG_t *pg);
extern int PG_nfields (PG_t *pg);
extern char *PG_fname (PG_t *pg, int field);
extern char *PG_value (PG_t *pg, int tuple, int field);

extern io_t *PG_open (PG_t *pg, const char *name, const char *mode);

extern mdmat_t *PG_mdmat (PG_t *pg, const Type_t *type,
	const char *value, const char *axis);

extern int PG_expandlim;
extern void SetupPG (void);
extern void _init (void);

/*
$SeeAlso
\mref{refdata(3)},
\mref{PG(3)},
\mref{PG_exec(3)},
\mref{PG_open(3)},
\mref{PG_query(3)},
\mref{SetupPG(3)}.\br
PostgreSQL Programmer's Guide.
*/

#endif	/* DB/PG.h */
