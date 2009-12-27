/*
Arbeiten mit Datenbankdaten

$Copyright (C) 2002 Erich Fr�hst�ck
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
#include <ctype.h>

#define	BSIZE	2040

#define	ALIGN(x,s)	((s) * (1 + ((x) / (s))))

#define	MSG_POS	"[db:pos]" \
"$!: DBData: field position $1, width $2 out of range (recl = $3).\n"
#define	MSG_IDX	"[db:idx]" \
"$!: DBData: field index $1 out of range (dim = $2).\n"

static DBDATA(DBDataBuf);

/*
Die Funktion |$1| liest einen EBCDIC-Datensatz der L�nge <recl>.
Falls <recl> gleich 0 ist, wird von einem Datensatz mit Satzl�ngenfeld
ausgegangen. Bei erfolgreichem Lesen liefert die Funktion einen
Pointer auf den Datenbuffer. Bei einem Fehler oder am Ende der
Datei wird ein NULL-Pointer zur�ckgegeben.
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
			io_error(io, "[db:3]", NULL);
			return NULL;
		}
			
		if	(io_getval(io, 2) != 0)
		{
			io_error(io, "[db:4]", NULL);
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
		io_error(io, "[db:4]", NULL);
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
Ein Wagenr�cklauf in der Eingabedatei wird ignoriert.
Ein NUL-Zeichen wird durch ein Leerzeichen esrsetzt.
Falls eine Liste von Trennzeichen <delim> angegeben wurde,
wird der Datensatz in Datenfelder aufgespalten.
Bei erfolgreichem Lesen liefert die Funktion einen
Pointer auf den Datenbuffer. Bei einem Fehler oder am Ende der
Datei wird ein NULL-Pointer zur�ckgegeben.
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
Die Funktion |$1| synchronisiert den Datenbuffer auf die Satzl�nge <recl>.
Bei Verl�ngerung wird mit dem Zeichen <c> aufgef�llt.
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
die zul�ssigen Trennzeichen angibt. Falls <delim> kein Leerzeichen enth�lt,
wird jedes Feld durch genau ein Trennzeichen getrennt. Ansonsten gilt
eine beliebige Folge von Leerzeichen und Tabulatoren und optional ein
sonstiges Trennzeichen als Feldtrennung.
Die Funktion liefert die Zahl der Felder und legt ein zus�tzliches,
leeres Feld f�r missing Values an.
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
Position pr�fen pr�fen
*/

int DBData_check (DBData *data, int pos, int len)
{
	if	(data == NULL || pos < 1)
		return 1;

	if	(pos + len - 1 <= data->recl)
		return 0;

	dbg_note("DBData", MSG_POS, "ddd", pos, len, (int) data->recl);
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

	dbg_note("DBData", MSG_POS, "ddd", pos, len, (int) data->recl);
	return NULL;
}

/*
Die Funktion |$1| liefert das Datenfeld mit Index <idx>.
Falls <data> ein Nullpointer ist, wird ein Nullpointer
zur�ckgegeben. Falls Index <idx> au�erhalb des zul�ssigen Bereichs liegt,
wird Ein Leerstring zur�ckgegeben.
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
Falls Index <idx> au�erhalb des zul�ssigen Bereichs liegt, wird
zus�tzlich eine Warnung ausgegeben.
*/
		
char *DBData_xfield (DBData *data, int idx)
{
	if	(data == NULL || idx < 1 || idx > data->dim)
	{
		dbg_note("DBData", MSG_IDX,
			"dd", idx + 1, data ? (int) data->dim : 0);
	}

	return DBData_field(data, idx);
}

/*
Die Funktion |$1| liefert den Kalenderindeg zu einer Datumsangabe
im Datenfeld mit Index <idx>. Falls <data> ein Nullpointer ist,
oder <idx> au�erhalb des zul�ssigen Bereichs liegt, wird 0 zur�ckgegeben.
Falls <flag> ungleich null ist, wird bei der Datumsvervollst�ndigung
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
