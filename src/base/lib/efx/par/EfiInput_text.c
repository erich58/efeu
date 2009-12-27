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
#include <EFEU/DBData.h>
#include <EFEU/parsearg.h>
#include <EFEU/EfiInput.h>
#include <ctype.h>

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

#define ISVEC(t)	((t)->dim || IsTypeClass(t, &Type_vec))

static int read_label (EfiType *type, void *data, void *par)
{
	DBData *db;
	EfiStruct *var;
	EfiObj *obj;
	EfiObj *base;
	IO *io;
	int i;

	io = par;

	if	(start_line(io) == EOF)
		return 0;

	if	((db = DBData_qtext(NULL, io, ";\t")) == NULL)
		return 0;

	base = LvalObj(&Lval_ptr, type, data);

	if	(!type->list)
	{
		obj = label_obj(DBData_field(db, 1));
		UnrefObj(AssignObj(base, obj));
		return 1;
	}

	for (i = 1, var = type->list; var != NULL; var = var->next, i++)
	{
		obj = label_obj(DBData_field(db, i));
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
