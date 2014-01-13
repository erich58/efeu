/*
Arbeiten mit Datenbankdaten

$Copyright (C) 2002 Erich Frühstück
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

#include <EFEU/DBData.h>
#include <EFEU/Debug.h>
#include <EFEU/calendar.h>
#include <EFEU/ioscan.h>
#include <ctype.h>

#define	BSIZE	2040

#define	ALIGN(x,s)	((s) * (1 + ((x) / (s))))

#define	MSG_POS	"[db:pos]" \
"$!: DBData: field position $1, width $2 out of range (recl = $3).\n"
#define	MSG_IDX	"[db:idx]" \
"$!: DBData: field index $1 out of range (dim = $2).\n"
#define	MSG_RECL	"[db:recl]" \
"$!: DBData: invalid record length.\n"

static LogControl note_ctrl = LOG_CONTROL("DBData", LOGLEVEL_NOTE);
static DBDATA(DBDataBuf);

/*
Die Funktion |$1| liest einen EBCDIC-Datensatz der Länge <recl>.
Falls <recl> gleich 0 ist, wird von einem Datensatz mit Satzlängenfeld
ausgegangen. Bei erfolgreichem Lesen liefert die Funktion einen
Pointer auf den Datenbuffer. Bei einem Fehler oder am Ende der
Datei wird ein NULL-Pointer zurückgegeben.
*/

DBData *DBData_ebcdic (DBData *db, IO *io, int recl)
{
	if	(db == NULL)
		db = &DBDataBuf;
	
	db->recl = 0;
	db->dim = 0;
	db->ebcdic = 1;

	if	(io_peek(io) == EOF)
		return NULL;

	if	(!recl)
	{
		db->recl = io_getval(io, 2);

		if	(db->recl < 4)
		{
			io_error(io, MSG_RECL, NULL);
			return NULL;
		}
			
		if	(io_getval(io, 2) != 0)
		{
			io_error(io, MSG_RECL, NULL);
			return NULL; 
		}

		db->recl -= 4;
		db->fixed = 0;
	}
	else
	{
		db->fixed = 1;
		db->recl = recl;
	}

	if	(db->buf_size <= db->recl)
	{
		lfree(db->buf);
		db->buf_size = ALIGN(db->recl, BSIZE);
		db->buf = lmalloc(db->buf_size);
	}

	if	(io_read(io, db->buf, db->recl) != db->recl)
	{
		io_error(io, MSG_RECL, NULL);
		return NULL;
	}

	db->buf[db->recl] = 0;
	return db;
}

static void DBData_expand (DBData *db)
{
	void *buf = db->buf;
	db->buf_size += BSIZE;
	db->buf = lmalloc(db->buf_size);
	memcpy(db->buf, buf, db->recl);
	lfree(buf);
}

/*
Die Funktion |$1| ladet eine Datenzeile im Textformat.
Ein Wagenrücklauf in der Eingabedatei wird ignoriert.
Ein NUL-Zeichen wird durch ein Leerzeichen ersetzt.
Falls eine Liste von Trennzeichen <delim> angegeben wurde,
wird der Datensatz in Datenfelder aufgespalten.
Bei erfolgreichem Lesen liefert die Funktion einen
Pointer auf den Datenbuffer. Bei einem Fehler oder am Ende der
Datei wird ein NULL-Pointer zurückgegeben.
*/

DBData *DBData_text (DBData *db, IO *io, const char *delim)
{
	int c;

	if	(db == NULL)
		db = &DBDataBuf;
	
	db->recl = 0;
	db->dim = 0;
	db->ebcdic = 0;
	db->fixed = 0;

	if	(io_peek(io) == EOF)
		return NULL;

	while ((c = io_getc(io)) != '\n')
	{
		if	(c == EOF)	break;
		if	(c == '\r')	continue;
		if	(c == 0)	c = ' ';

		if	(db->buf_size <= db->recl)
			DBData_expand(db);

		db->buf[db->recl++] = c;
	}

	if	(db->buf_size <= db->recl)
		DBData_expand(db);

	db->buf[db->recl] = 0;

	if	(delim)
		DBData_split (db, delim);

	return db;
}

static void add_qfield (DBData *db, IO *io)
{
	int c;

	while ((c = io_getc(io)) != EOF)
	{
		if	(c == '\n')
		{
			io_ungetc(c, io);
			return;
		}

		if	(c == '"')
		{
			if	((c = io_getc(io)) != '"')
			{
				io_ungetc(c, io);
				return;
			}
		}

		if	(db->buf_size <= db->recl)
			DBData_expand(db);

		db->buf[db->recl++] = c;
	}
}

/*
Die Funktion |$1| ladet eine Datenzeile im Textformat und
spaltet sie nach den Trennzeichen auf. Im Unterschied zu
|DBData_text| werden Anführungszeichen berücksichtigt.
Die Aufspaltung erfolgt bereits beim Laden der Datenzeile.
*/

DBData *DBData_qtext (DBData *db, IO *io, const char *delim)
{
	char *p;
	int mode;
	int need_space;
	int c;
	int i;

	if	(db == NULL)
		db = &DBDataBuf;
	
	if	(delim == NULL)	delim = " \t;";

	db->recl = 0;
	db->dim = 0;
	db->ebcdic = 0;
	db->fixed = 0;

	if	(io_peek(io) == EOF)
		return NULL;

	mode = 0;
	need_space = 0;

	while (mode != EOF)
	{
		c = io_getc(io);

		if	(c == '\r')	continue;
		if	(c == 0)	c = ' ';

		if	(mode == 0 && c == ' ')	
			continue;

		if	(c == '\n' || c == EOF)
		{
			db->dim++;
			mode = EOF;
			c = 0;
		}
		else if	(strchr(delim, c))
		{
			db->dim++;
			mode = 0;

			while (c == ' ' || c == '\t')
				c = io_getc(io);

			if	(c == '\n' || c == EOF)
				mode = EOF;
			else if	(strchr(delim, c) == NULL)
				io_ungetc(c, io);

			need_space = 0;
			c = 0;
		}
		else if	(mode == 0 && c == '"')
		{
			add_qfield(db, io);
			mode = 1;
			need_space = 0;
			continue;
		}
		else if	(c == ' ')
		{
			need_space = 1;
			continue;
		}
		else	mode = 1;

		if	(need_space)
		{
			if	(db->buf_size <= db->recl)
				DBData_expand(db);

			db->buf[db->recl++] = ' ';
			need_space = 0;
		}

		if	(db->buf_size <= db->recl)
			DBData_expand(db);

		db->buf[db->recl++] = c;
	}

	if	(db->tab_size < db->dim)
	{
		db->tab_size = sizealign(db->dim, BSIZE);
		lfree(db->tab);
		db->tab = lmalloc(db->tab_size * sizeof (char *));
	}

	p = (char *) db->buf;
	db->tab[0] = p;

	for (i = 1; i < db->dim; i++)
	{
		while (*p != 0)
			p++;

		db->tab[i] = ++p;
	}

	return db;
}

/*
Die Funktion |$1| konvertiert EBCDIC-Daten im Datenbuffer in ein Textformat.
*/

void DBData_conv (DBData *db)
{
	if	(db && db->ebcdic)
	{
		int i;

		for (i = 0; i < db->recl; i++)
			db->buf[i] = ebcdic2ascii(db->buf[i]);

		db->ebcdic = 0;
	}
}

/*
Die Funktion |$1| synchronisiert den Datenbuffer auf die Satzlänge <recl>.
Bei Verlängerung wird mit dem Zeichen <c> aufgefüllt.
*/

void DBData_sync (DBData *db, int recl, int c)
{
	if	(db == NULL)	return;

	if	(db->recl < recl)
	{
		if	(db->buf_size <= recl)
		{
			void *buf = db->buf;
			db->buf_size = ALIGN(recl, BSIZE);
			db->buf = lmalloc(db->buf_size);
			memcpy(db->buf, buf, db->recl);
			lfree(buf);
		}

		memset(db->buf + db->recl, c, recl - db->recl);
	}

	db->recl = recl;
	db->buf[db->recl] = 0;
}

/*
Die Funktion |$1| zerlegt den Datenbuffer in einzelne Felder, wobei <delim>
die zulässigen Trennzeichen angibt. Falls <delim> kein Leerzeichen enthält,
wird jedes Feld durch genau ein Trennzeichen getrennt. Ansonsten gilt
eine beliebige Folge von Leerzeichen und Tabulatoren und optional ein
sonstiges Trennzeichen als Feldtrennung.
Die Funktion liefert die Zahl der Felder und legt ein zusätzliches,
leeres Feld für missing Values an.
*/

int DBData_split (DBData *db, const char *delim)
{
	char *p;
	int flag;

	if	(db == NULL)	return 0;
	if	(delim == NULL)	delim = " \t;";

	p = (char *) db->buf;
	db->dim = 0;
	flag = strchr(delim, ' ') ? 1 : 0;

	for (;;)
	{
		if	(db->tab_size <= db->dim)
		{
			void *tab = db->tab;
			db->tab_size += BSIZE;
			db->tab = lmalloc(db->tab_size * sizeof (char *));
			memcpy(db->tab, tab, db->dim * sizeof (char *));
			lfree(tab);
		}

		if	(flag)
		{
			while (*p == ' ' || *p == '\t')
				p++;
		}

		db->tab[db->dim] = p;

		if	(*p == 0)	break;

		db->dim++;

		while (*p)
		{
			if	(flag && (*p == ' ' || *p == '\t'))
			{
				while (*p == ' ' || *p == '\t')
				{
					*p = 0;
					p++;
				}

				if	(strchr(delim, *p) != NULL)
					p++;

				break;
			}
			else if	(strchr(delim, *p) != NULL)
			{
				*p = 0;
				p++;
				break;
			}
			else	p++;
		}
	}

	return db->dim;
}


/*
Position prüfen prüfen
*/

int DBData_check (DBData *data, int pos, int len)
{
	if	(data == NULL || pos < 1)
		return 1;

	if	(pos + len - 1 <= data->recl)
		return 0;

	log_note(&note_ctrl, MSG_POS, "ddd", pos, len, (int) data->recl);
	return 1;
}

/*	Pointer auf Datenwert
*/

unsigned char *DBData_ptr (DBData *data, int pos, int len)
{
	if	(data == NULL || pos < 1)	return NULL;

	pos--;

	if	(pos + len <= data->recl)
		return data->buf + pos;

	log_note(&note_ctrl, MSG_POS, "ddd", pos, len, (int) data->recl);
	return NULL;
}

/*
Die Funktion |$1| liefert das Datenfeld mit Index <idx>.
Falls <data> ein Nullpointer ist, wird ein Nullpointer
zurückgegeben. Falls Index <idx> außerhalb des zulässigen Bereichs liegt,
wird ein Leerstring zurückgegeben.
*/

char *DBData_field (DBData *data, int idx)
{
	if	(data == NULL)	return NULL;
	
	if	(idx < 1)	return "";

	idx--;

	if	(idx < data->dim)
		return data->tab[idx];

	return "";
}

/*
Die Funktion |$1| liefert das Datenfeld mit Index <idx>.
Falls Index <idx> außerhalb des zulässigen Bereichs liegt, wird
zusätzlich eine Warnung ausgegeben.
*/
		
char *DBData_xfield (DBData *data, int idx)
{
	if	(data == NULL || idx < 1 || idx > data->dim)
	{
		log_note(&note_ctrl, MSG_IDX,
			"dd", idx + 1, data ? (int) data->dim : 0);
	}

	return DBData_field(data, idx);
}

/*
Die Funktion |$1| liefert den Kalenderindeg zu einer Datumsangabe
im Datenfeld mit Index <idx>. Falls <data> ein Nullpointer ist,
oder <idx> außerhalb des zulässigen Bereichs liegt, wird 0 zurückgegeben.
Falls <flag> ungleich null ist, wird bei der Datumsvervollständigung
der letzte Tag, ansonsten der erste Tag im Monat/Jahr genommen.
*/

int DBData_date (DBData *data, int idx, int flag)
{
	if	(data == NULL || idx < 1)
		return 0;
	
	idx--;

	if	(idx < data->dim)
		return str2Calendar(data->tab[idx], NULL, flag);

	return 0;
}

/*
Gleitkommawert lesen
*/

double DBData_field_double (DBData *data, int pos)
{
	char *p = DBData_field(data, pos);

	if	(p && *p)
	{
		IO *io = io_cstr(p);
		void *ptr = NULL;

		io_valscan(io, SCAN_DOUBLE, &ptr);
		io_close(io);
		return *((double *) ptr);
	}

	return 0.;
}

/*	Test auf Leerzeichen
*/

int DBData_isblank (DBData *data, int pos, int len)
{
	unsigned char *ptr = DBData_ptr(data, pos, len);

	if	(ptr)
	{
		int space = data->ebcdic ? 0x40 : ' ';

		while (len-- > 0)
			if (*ptr++ != space) return 0;
	}

	return 1;
}

/* Test auf Zahlebnwert
*/

int DBData_isdigit (DBData *data, int pos, int len)
{
	unsigned char *ptr = DBData_ptr(data, pos, len);

	if	(!ptr)	return 1;

	if	(data->ebcdic)
	{
		for (; len-- > 0; ptr++)
			if (*ptr != 0x40 && !(*ptr & 0xF0))
				return 0;
	}
	else
	{
		for (; len-- > 0; ptr++)
			if (*ptr != ' ' && !isdigit(*ptr))
				return 0;
	}

	return 1;
}

unsigned DBData_uint (DBData *data, int pos, int len)
{
	if	(DBData_check(data, pos, len))
		return 0;

	if	(data->ebcdic)
		return db_cval(data->buf, pos, len);

	return txt_unsigned((char *) data->buf, pos, len);
}

unsigned DBData_base37 (DBData *data, int pos, int len)
{
	if	(DBData_check(data, pos, len))
		return 0;

	if	(data->ebcdic)
		return db_a37l(data->buf, pos, len);

	return txt_base37((char *) data->buf, pos, len);
}

char *DBData_str (DBData *data, int pos, int len)
{
	if	(DBData_check(data, pos, len))
		return 0;

	if	(data->ebcdic)
		return db_str(data->buf, pos, len);

	return txt_str((char *) data->buf, pos, len);
}

char *DBData_xstr (DBData *data, int pos, int len)
{
	if	(DBData_check(data, pos, len))
		return 0;

	if	(data->ebcdic)
		return db_str(data->buf, pos, len);

	return txt_xstr((char *) data->buf, pos, len);
}

int DBData_char (DBData *data, int pos, int len)
{
	char *ptr = (char *) DBData_ptr(data, pos, len);
	return ptr ? (data->ebcdic ? ebcdic2ascii(*ptr) : *ptr) : 0;
}

unsigned DBData_packed (DBData *data, int pos, int len)
{
	if	(DBData_check(data, pos, len))
		return 0;
	
	return db_pval(data->buf, pos, len);
}

unsigned DBData_bcdval (DBData *data, int pos, int len)
{
	if	(DBData_check(data, pos, len))
		return 0;
	
	return db_bcdval(data->buf, pos, len);
}

unsigned DBData_binary (DBData *data, int pos, int len)
{
	unsigned char *ptr = DBData_ptr(data, pos, len);
	unsigned val = 0;

	if	(ptr)
	{
		while (len-- > 0)
		{
			val <<= 8;
			val += *ptr++;
		}
	}

	return val;
}
