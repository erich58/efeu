/*
Datenbank laden

$Copyright (C) 1995 Erich Frühstück
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

#include <EFEU/database.h>

typedef struct {
	EfiObj *data;	/* Aktueller Datensatz */
	EfiObj *def;	/* Vorgabewert */
	EfiObj *last;	/* Letzter Eintrag */
	EfiObj *flag;	/* Flag zur Eintragssteuerung */
} LPAR;

/*	Neue Zeile beginnen
*/

static int nextline (IO *io)
{
	int c;
	int flag;

	flag = 0;

	while ((c = io_mgetc(io, 1)) != EOF)
	{
		if	(c == '\n')
		{
			if	(flag == 0)
			{
				io_ungetc(c, io);
				break;
			}
			else	flag = 0;
		}
		else if	(c == '#' || c == '%')
		{
			do 	c = io_getc(io);
			while	(c != '\n' && c != EOF);
		}
		else if	(c == '/' && iocpy_cskip(io, NULL, c, NULL, 1) != EOF)
		{
			flag = 1;
		}
		else if	(c != ' ' && c != '\t')
		{
			io_ungetc(c, io);
			break;
		}
	}

	return c;
}


/*	Datenwert in Datenbank eintragen
*/

static void addtodb (EfiDB *db, void *buf, EfiObj *expr)
{
	void *ptr;

	if	(expr && (ptr = DB_search(db, buf, VB_SEARCH)))
	{
		PushVarTab(NULL, NULL);
		VarTab_xadd(NULL, "x", NULL, LvalObj(&Lval_ptr, db->type, buf));
		VarTab_xadd(NULL, "y", NULL, LvalObj(&Lval_ptr, db->type, ptr));
		UnrefEval(RefObj(expr));
		PopVarTab();
		CleanData(db->type, buf);
	}
	else	DB_search(db, buf, VB_ENTER);
}


/*	Komplexe Datenbankeinträge
*/

static void load_complex (IO *io, EfiDB *db, LPAR *lpar, EfiObj *expr)
{
	int c;

	while ((c = nextline(io)) != EOF)
	{
		if	(c == '\n')
		{
			io_getc(io);
			
			if	(Val_bool(lpar->flag->data))
			{
				AssignData(db->type, lpar->last->data,
					lpar->data->data);
				addtodb(db, lpar->data->data, expr);
				Val_bool(lpar->flag->data) = 0;
			}

			AssignData(db->type, lpar->data->data,
				lpar->def->data);
		}
		else
		{
			Val_bool(lpar->flag->data) = 1;
			UnrefEval(Parse_cmd(io));

			if	(io_eat(io, " \t;") == '\n')
				io_getc(io);
		}
	}

	if	(Val_bool(lpar->flag->data))
		addtodb(db, lpar->data->data, expr);
}


/*	Kompakte Datenbankeinträge
*/

static void load_compact (IO *io, EfiDB *db, LPAR *lpar, EfiObj *expr)
{
	EfiObjList *list, *x, **ptr;
	EfiObj *obj;
	int last;
	int c;
	int header;

	x = list = NULL;
	header = 0;
	ptr = NULL;
	Val_bool(lpar->flag->data) = 0;

	for (last = '\n'; (c = io_eat(io, " \t")) != EOF; last = c)
	{
		if	(c == '\n')
		{
			if	(Val_bool(lpar->flag->data))
			{
				AssignData(db->type, lpar->last->data,
					lpar->data->data);
				addtodb(db, lpar->data->data, expr);
			}

			io_getc(io);
			x = list;
			header = 0;
			Val_bool(lpar->flag->data) = 0;
		}
		else if	(c == ':' || c == ';')
		{
			io_getc(io);

			if	(x)	x = x->next;
		}
		else if	(last == '\n' && c == '#')
		{
			io_eat(io, "# \t");
			header = 1;
			DelObjList(list);
			ptr = &list;
			x = NULL;
		}
		else
		{
			obj = Parse_term(io, 0);

			if	(header)
			{
				*ptr = NewObjList(EvalObj(obj, NULL));
				ptr = &(*ptr)->next;
			}
			else if	(x && x->obj)
			{
				AssignObj(x->obj, obj);
				Val_bool(lpar->flag->data) = 1;
			}
			else	UnrefEval(obj);
		}
	}
}


/*	Datenbankwerte laden
*/

void DB_load (IO *io, EfiDB *db, EfiObj *expr)
{
	LPAR lpar;

	if	(io == NULL || db == NULL)	return;

/*	Variablen zusammenstellen
*/
	lpar.data = LvalObj(NULL, db->type);
	lpar.def = LvalObj(NULL, db->type);
	lpar.last = LvalObj(NULL, db->type);
	lpar.flag = LvalObj(NULL, &Type_bool);

	PushVarTab(RefVarTab(db->type->vtab), lpar.data);
	PushVarTab(NULL, NULL);
	VarTab_xadd(NULL, "default", NULL, lpar.def);
	VarTab_xadd(NULL, "last", NULL, lpar.last);
	VarTab_xadd(NULL, "addentry", NULL, lpar.flag);
	PushContext(RefVarTab(LocalVar), NULL);

/*	Datenbankeinträge lesen
*/
	if	(io_eat(io, ";%s") == '#')
	{
		load_compact(io, db, &lpar, expr);
	}
	else	load_complex(io, db, &lpar, expr);

/*	Aufräumen
*/
	PopContext();
	PopVarTab();
	PopVarTab();
}
