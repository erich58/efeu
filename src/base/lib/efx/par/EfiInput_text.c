/*
Daten im Textformat einlesen

$Copyright (C) 2006 Erich Frühstück
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

#include <EFEU/ioctrl.h>
#include <EFEU/printobj.h>
#include <EFEU/parsearg.h>
#include <EFEU/EfiInput.h>
#include <ctype.h>

typedef struct {
	unsigned recl;		/* Satzlänge */
	unsigned dim;		/* Felddimension */
	unsigned char *buf;	/* Datenbuffer */
	char **tab;		/* Feldvektor */
	unsigned buf_size;	/* Buffergröße */
	unsigned tab_size;	/* Tabellengröße */
} LabelData;

#define	BSIZE	2040

static LabelData LabelDataBuf = { 0, 0, NULL, NULL, 0, 0 };

static void LabelData_expand (LabelData *db)
{
	void *buf = db->buf;
	db->buf_size += BSIZE;
	db->buf = lmalloc(db->buf_size);
	memcpy(db->buf, buf, db->recl);
	lfree(buf);
}

/*
Die Funktion |$1| liefert das Datenfeld mit Index <idx>.
Falls <data> ein Nullpointer ist, wird ein Nullpointer
zurückgegeben. Falls Index <idx> außerhalb des zulässigen Bereichs liegt,
wird ein Leerstring zurückgegeben.
*/

static char *LabelData_field (LabelData *data, int idx)
{
	if	(data == NULL)	return NULL;
	
	if	(idx < 1)	return "";

	idx--;

	if	(idx < data->dim)
		return data->tab[idx];

	return "";
}

static void add_qfield (LabelData *db, IO *io)
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
			LabelData_expand(db);

		db->buf[db->recl++] = c;
	}
}


/*
Die Funktion |$1| ladet eine Datenzeile im Textformat und
spaltet sie nach den Trennzeichen auf. Im Unterschied zu
|LabelData_text| werden Anführungszeichen berücksichtigt.
Die Aufspaltung erfolgt bereits beim Laden der Datenzeile.
*/

static LabelData *LabelData_qtext (LabelData *db, IO *io, const char *delim)
{
	char *p;
	int mode;
	int need_space;
	int c;
	int i;

	if	(db == NULL)
		db = &LabelDataBuf;
	
	if	(delim == NULL)	delim = " \t;";

	db->recl = 0;
	db->dim = 0;

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
				LabelData_expand(db);

			db->buf[db->recl++] = ' ';
			need_space = 0;
		}

		if	(db->buf_size <= db->recl)
			LabelData_expand(db);

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

static int start_line (IO *io)
{
	int c;

	while ((c = io_mgetc(io, 1)) != EOF)
	{
		if	(c == '/' && iocpy_cskip(io, NULL, c,
				NULL, 1) != EOF)
		{
			continue;
		}
		else if	(c == '#')
		{
			do	c = io_mgetc(io, 1);
			while	(c != '\n' && c != EOF);
		}
		else if	(!isspace(c))
		{
			io_ungetc(c, io);
			break;
		}
	}

	return c;
}

static EfiObj *label_obj (char *label)
{
	return *label == '{' ? strterm(label) : str2Obj(mstrcpy(label));
}

static int read_label (EfiType *type, void *data, void *par)
{
	LabelData *db;
	EfiStruct *var;
	EfiObj *obj;
	EfiObj *base;
	IO *io;
	int i;

	io = par;

	if	(start_line(io) == EOF)
		return 0;

	if	((db = LabelData_qtext(NULL, io, ";\t")) == NULL)
		return 0;

	base = LvalObj(&Lval_ptr, type, data);

	if	(!type->list)
	{
		obj = label_obj(LabelData_field(db, 1));
		UnrefObj(AssignObj(base, obj));
		return 1;
	}

	for (i = 1, var = type->list; var != NULL; var = var->next, i++)
	{
		obj = label_obj(LabelData_field(db, i));
		UnrefObj(AssignObj(Var2Obj(var, base), obj));
	}

	UnrefObj(base);
	return 1;
}

static int read_plain (EfiType *type, void *data, void *par)
{
	EfiObj *base;
	EfiObj *obj;
	EfiStruct *var;
	IO *io;
	int c;

	io = par;

	if	((c = start_line(io)) == EOF)
		return 0;

	base = LvalObj(&Lval_ptr, type, data);

	if	(!type->list)
	{
		UnrefObj(AssignObj(base, Parse_term(io, 0)));
		io_getc(io);
		return 1;
	}

	var = type->list;

	while (c != '\n' && c != EOF)
	{
		obj = Parse_term(io, 0);

		if	(var)
		{
			obj = AssignObj(Var2Obj(var, base), obj);
			var = var->next;
		}

		UnrefObj(obj);
		c = io_getc(io);
	}

	while (var)
	{
		obj = Var2Obj(var, base);
		CleanData(obj->type, obj->data, 0);
		UnrefObj(obj);
		var = var->next;
	}

	UnrefObj(base);
	return 1;
}

static void *open_text (IO *io, const EfiType *type,
	void *par, const char *opt, const char *arg)
{
	if	(opt)
	{
		unsigned n = strtoul(opt, NULL, 10);

		while (n--)
		{
			int c;

			do	c = io_getc(io);
			while	(c != '\n' && c != EOF);
		}
	}

	return io;
}

EfiInput EfiInput_label = {
	EPC_DATA(&EfiPar_input, NULL, "label", "label text input", NULL),
	open_text, read_label, NULL,
};

EfiInput EfiInput_plain = {
	EPC_DATA(&EfiPar_input, NULL, "plain", "plain text input", NULL),
	open_text, read_plain, NULL,
};
