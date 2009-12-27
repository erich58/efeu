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

#define	THIS		0	/* Aktueller Datensatz */
#define	DEFAULT		1	/* Vorgabewert */
#define	LAST		2	/* Letzter Eintrag */
#define	DB_VAR		3	/* Datenbankvariablen */

#define	ADDENTRY	3	/* Flag zur Eintragssteuerung */
#define	EXTRA_VAR	1	/* Zusatzvariablen */


/*	Neue Zeile beginnen
*/

static int nextline (io_t *io)
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

static void addtodb(DataBase_t *db, void *buf, Obj_t *expr)
{
	void *ptr;

	if	(expr && (ptr = DB_search(db, buf, VB_SEARCH)))
	{
		Var_t z[2];

		memset(z, 0, 2 * sizeof(Var_t));
		z[0].name = "y";
		z[0].type = db->type;
		z[0].data = ptr;
		z[0].dim = 0;
		z[1].name = "x";
		z[1].type = db->type;
		z[1].data = buf;
		z[1].dim = 0;
		PushVarTab(NULL, NULL);
		AddVar(NULL, z, 2);
		UnrefEval(RefObj(expr));
		PopVarTab();
		CleanData(db->type, buf);
	}
	else	DB_search(db, buf, VB_ENTER);
}


/*	Komplexe Datenbankeinträge
*/

static void add_complex(DataBase_t *db, Var_t *var, Obj_t *expr)
{
	AssignData(db->type, var[LAST].data, var[THIS].data);

	if	(Val_bool(var[ADDENTRY].data))
	{
		addtodb(db, var[THIS].data, expr);
		Val_bool(var[ADDENTRY].data) = 0;
	}
}

static void load_complex(io_t *io, DataBase_t *db, Var_t *var, Obj_t *expr)
{
	int c;

	while ((c = nextline(io)) != EOF)
	{
		if	(c == '\n')
		{
			io_getc(io);
			add_complex(db, var, expr);
			AssignData(db->type, var[THIS].data, var[DEFAULT].data);
		}
		else
		{
			Val_bool(var[ADDENTRY].data) = 1;
			UnrefEval(Parse_cmd(io));

			if	(io_eat(io, " \t;") == '\n')
				io_getc(io);
		}
	}

	add_complex(db, var, expr);
}


/*	Kompakte Datenbankeinträge
*/

static void load_compact(io_t *io, DataBase_t *db, Var_t *var, Obj_t *expr)
{
	ObjList_t *list, *x, **ptr;
	Obj_t *obj;
	int last;
	int c;
	int header;

	x = list = NULL;
	header = 0;
	ptr = NULL;
	Val_bool(var[ADDENTRY].data) = 0;

	for (last = '\n'; (c = io_eat(io, " \t")) != EOF; last = c)
	{
		if	(c == '\n')
		{
			if	(Val_bool(var[ADDENTRY].data))
			{
				AssignData(db->type, var[LAST].data,
					var[THIS].data);
				addtodb(db, var[THIS].data, expr);
			}

			io_getc(io);
			x = list;
			header = 0;
			Val_bool(var[ADDENTRY].data) = 0;
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
				Val_bool(var[ADDENTRY].data) = 1;
			}
			else	UnrefEval(obj);
		}
	}
}


/*	Datenbankwerte laden
*/

void DB_load(io_t *io, DataBase_t *db, Obj_t *expr)
{
	Var_t *var;
	char *buf;
	int nvar;
	int addentry;
	Var_t *st;

	if	(io == NULL || db == NULL)	return;

/*	Variablentabelle aufbauen
*/
	buf = memalloc(DB_VAR * db->type->size);
	memset(buf, 0, DB_VAR * db->type->size);
	nvar = DB_VAR + EXTRA_VAR;

	for (st = db->type->list; st != NULL; st = st->next)
		nvar++;

	var = ALLOC(nvar, Var_t);
	memset(var, 0, nvar * sizeof(Var_t));
	var[THIS].type = db->type;
	var[THIS].name = "this";
	var[THIS].data = buf + THIS * db->type->size;
	var[THIS].dim = 0;
	var[DEFAULT].type = db->type;
	var[DEFAULT].name = "default";
	var[DEFAULT].data = buf + DEFAULT * db->type->size;
	var[DEFAULT].dim = 0;
	var[LAST].type = db->type;
	var[LAST].name = "last";
	var[LAST].data = buf + LAST * db->type->size;
	var[LAST].dim = 0;
	var[ADDENTRY].type = &Type_bool;
	var[ADDENTRY].name = "addentry";
	var[ADDENTRY].data = &addentry;
	var[ADDENTRY].dim = 0;
	nvar = DB_VAR + EXTRA_VAR;

	for (st = db->type->list; st != NULL; st = st->next)
	{
		var[nvar].type = st->type;
		var[nvar].name = st->name;
		var[nvar].data = buf + st->offset;
		var[nvar].dim = st->dim;
		nvar++;
	}

	PushVarTab(NULL, NULL);
	AddVar(NULL, var, nvar);
	PushContext(RefVarTab(LocalVar), NULL);

/*	Datenbankeinträge lesen
*/
	if	(io_eat(io, ";%s") == '#')
	{
		load_compact(io, db, var, expr);
	}
	else	load_complex(io, db, var, expr);

/*	Aufräumen
*/
	CleanData(db->type, var[THIS].data);
	CleanData(db->type, var[DEFAULT].data);
	CleanData(db->type, var[LAST].data);
	PopContext();
	PopVarTab();
	memfree(var);
	memfree(buf);
}
