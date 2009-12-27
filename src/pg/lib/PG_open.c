/*
:de:	Interface für Datenübermittlung öffnen
:*:	open interface for copying data to/from database server

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
#include <EFEU/strbuf.h>
#include <EFEU/mstring.h>
#include <EFEU/ioctrl.h>
#include <EFEU/Debug.h>

#define	M_OPEN	"$!: PG_open(): unknown access mode \"$2\" for table \"$1\".\n"
#define	M_FLUSH	"$!: PQputline() failed.\n"

#define	BSIZE	1024	/* Größe des Eingabebuffers */

typedef struct {
	PG_t *pg;	/* Datenbankstruktur */
	char *cmd;	/* SQL-Kommando */
	strbuf_t *wbuf;	/* Ausgabebuffer */
	char *rbuf;	/* Eingabebuffer */
	char *ptr;	/* Pointer auf Eingabebuffer */
	int flag;	/* Flag für offene Daten */
} PGCOPY;

static void pg_flush (PGCOPY *par)
{
	if	(sb_getpos(par->wbuf) == 0)
		return;

	sb_putc('\n', par->wbuf);
	sb_putc(0, par->wbuf);

	if      (PQputline(par->pg->conn, (char *) par->wbuf->data) != 0)
	{
		Message("PG", DBG_ERR, M_FLUSH, NULL);
	}

	sb_begin(par->wbuf);
}
				

static int pg_put (int c, PGCOPY *par)
{
	if	(c == '\n')
	{
		pg_flush(par);
	}
	else	sb_putc(c, par->wbuf);

	return c;
}

static void pg_load (PGCOPY *par, int flag)
{
	int c = PQgetline(par->pg->conn, par->rbuf, BSIZE);

	if	(c == 0 && flag && strcmp(par->rbuf, "\\.") == 0)
		c = EOF;

	if	(c == EOF)	*par->rbuf = 0;

	par->ptr = par->rbuf;
	par->flag = c;
}

static int pg_get (PGCOPY *par)
{
	while (*par->ptr == 0)
	{
		switch (par->flag)
		{
		case EOF:
			return EOF;
		case 0:
			pg_load(par, 1);
			return '\n';
		default:
			pg_load(par, 0);
			break;
		}
	}

	return *par->ptr++;
}

static int pg_ctrl (PGCOPY *par, int req, va_list list)
{
	switch (req)
	{
	case IO_CLOSE:

		if	(par->wbuf)
		{
			pg_flush(par);
			sb_puts("\\.", par->wbuf);
			pg_flush(par);
			del_strbuf(par->wbuf);
		}

		if	(par->rbuf)
		{
			while (par->flag != EOF)
				pg_load(par, par->flag == 0);

			memfree(par->rbuf);
		}

		if      (PQendcopy(par->pg->conn) != 0)
			Message("PG", DBG_ERR, "PQendcopy() failed\n", NULL);

		par->pg->lock = 0;
		PG_clear(par->pg);
		rd_deref(par->pg);
		memfree(par->cmd);
		memfree(par);
		return 0;

	case IO_IDENT:

		*va_arg(list, char **) = mstrcpy(par->cmd);
		return 0;

	default:

		break;
	}

	return EOF;
}

/*
:de:
Die Funktion |$1| führt eine COPY Anfrage an den Datenbankserver
durch und liefert eine IO-Struktur zur Übermittlung/Abfrage
von Daten. Die Verbindungsstruktur <pg> sollte danach nicht mehr
verwendet werden, außer der Referenzzähler wurde zuvor mit |rd_refer|
erhöht.

Der Parameter <mode> beginnt mit dem Kennbuchstaben
|r| (Daten fom Server lesen)
oder |w| (Daten zum Server schreiben) und der optionalen Kennung
|o| (mit Objektidentifikation).
:*:
The function |$1| performs a COPY command and returns an IO-structure
for copying data to/from the backend.
You should not use <pg> after calling |$1|, except you have incremented the
reference counter of <pg> with |rd_refer| before.

The parameter <mode> starts with the key
|r| (read data from server) or |w| (write data to server) and the
optional key |o| (copy the internal uniq object id for each row).

$Diagnostic
:de:
Die Funktion |$1| liefert einen NULL-Pointer, falls die
Anfrage fehlschlug.
Der Rückgabewert muß nicht geprüft werden, ein NULL-Pointer wird
wie |/dev/null| behandelt.
:*:
The function |$1| returns a NULL-Pointer on failure.
You need not check the return value, a NULL-pointer acts like |/dev/null|.

$Notes
:de:
Beim Schließen der IO-Struktur werden alle noch offenen Daten
vom Datenbankserver überlesen und die Verbindung bei Bedarf geschlossen,
Solange die IO-Struktur geöffnet ist, ist die Datenbankverbindung
für andere Abfragen gesperrt.
:*:
On closing, all pending data from backend are skipped and the connection
is closed on demand.
Until the IO-structure is open, the database connection <pg> is locked
for other execution commands.
*/

io_t *PG_open (PG_t *pg, const char *name, const char *mode)
{
	PGCOPY *par;
	const char *p;
	io_t *io;
	char *cmd;
	int wflag;
	char *oid;
	int stat;

	if	(pg == NULL)	return NULL;
	
	if	(name == NULL || mode == NULL)
	{
		rd_deref(pg);
		return NULL;
	}

	wflag = -1;
	oid = NULL;

	for (p = mode; *p != 0; p++)
	{
		switch (*p)
		{
		case 'w':	wflag = 1; break;
		case 'r':	wflag = 0; break;
		case 'o':	oid = "WITH OIDS"; break;
		default:	break;
		}
	}

	if	(wflag < 0)
	{
		Message("PG", DBG_ERR, M_OPEN, ArgList("ncc", name, mode));
		rd_deref(pg);
		return NULL;
	}
	else if	(wflag)
	{
		cmd = msprintf("COPY %s %s from stdin", name, oid);
		stat = PG_exec(pg, cmd, PGRES_COPY_IN);
	}
	else
	{
		cmd = msprintf("COPY %s %s to stdin", name, oid);
		stat = PG_exec(pg, cmd, PGRES_COPY_OUT);
	}

	if	(!stat)
	{
		memfree(cmd);
		return NULL;
	}

	pg->lock = 1;

	par = memalloc(sizeof(PGCOPY));
	par->pg = pg;
	par->cmd = cmd;

	io = io_alloc();

	if	(wflag)
	{
		par->wbuf = new_strbuf(0);
		io->put = (io_put_t) pg_put;
	}
	else
	{
		par->rbuf = memalloc(BSIZE);
		pg_load(par, 1);
		io->get = (io_get_t) pg_get;
	}

	io->ctrl = (io_ctrl_t) pg_ctrl;
	io->data = par;
	return io;
}

/*
$SeeAlso
\mref{io_getc(3)},
\mref{io_gets(3)},
\mref{io_puts(3)},
\mref{io_printf(3)},
\mref{PG(3)},
\mref{PG_exec(3)},
\mref{PG_query(3)},
\mref{SetupPG(3)},
\mref{io(7)}, \mref{PG(7)}.\br
PostgreSQL Programmer's Guide.
*/
