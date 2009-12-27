/*
Switch-Statement

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

#include <EFEU/object.h>
#include <EFEU/parsedef.h>
#include <EFEU/cmdeval.h>
#include <EFEU/stdtype.h>
#include <EFEU/Op.h>
#include <EFEU/printobj.h>


/*	Datentype für Klausel
*/

typedef struct Case_s Case_t;

struct Case_s {
	Obj_t *obj;
	Case_t *next;
	ObjList_t *list;
};

static ALLOCTAB(case_tab, 0, sizeof(Case_t));

static Case_t *new_case (Obj_t *obj)
{
	Case_t *data;

	data = new_data(&case_tab);
	data->obj = obj;
	data->next = NULL;
	data->list = NULL;
	return data;
}

static void del_case (Case_t *ptr)
{
	if	(ptr)
	{
		del_case(ptr->next);
		UnrefObj(ptr->obj);
		del_data(&case_tab, ptr);
	}
}


/*	Objekttype für Klausel
*/

#define	Val_case(x)	((Case_t **) (x))[0]
#define	Obj_case(x)	NewPtrObj(&Type_case, x)

static Obj_t *eval_case (const Type_t *type, const void *data)
{
	return NULL;
}

static void clean_case (const Type_t *st, void *data)
{
	del_case(Val_case(data));
	Val_case(data) = NULL;
}

static void copy_case (const Type_t *st, void *tg, const void *src)
{
	Val_case(tg) = new_case(RefObj(Val_case(src)->obj));
}

static Type_t Type_case = EVAL_TYPE("_case_", Case_t *,
	eval_case, clean_case, copy_case);


/*	Auswertungsfunktion
*/

static void eval_list (ObjList_t *list)
{
	PushVarTab(NULL, NULL);

	for (CmdEval_stat = 0; list && !CmdEval_stat; list = list->next)
		UnrefEval(RefObj(list->obj));

	if	(CmdEval_stat == CmdEval_Break)
		CmdEval_stat = 0;

	PopVarTab();
}

static Obj_t *Expr_switch (void *par, const ObjList_t *args)
{
	Obj_t *val, *x;
	Case_t *entry, *ptr, *def;

	val = EvalObj(RefObj(args->obj), NULL);
	entry = Val_case(args->next->obj->data);

	def = NULL;

	for (ptr = entry; ptr != NULL; ptr = ptr->next)
	{
		if	(ptr->obj == NULL)
		{
			def = ptr;
			continue;
		}

		x = EvalObj(BinaryTerm("==", RefObj(val),
			RefObj(ptr->obj)), NULL);

		if	(x && Obj2bool(x))
		{
			eval_list(ptr->list);
			UnrefObj(val);
			return NULL;
		}
	}

	if	(def)
		eval_list(def->list);

	UnrefObj(val);
	return NULL;
}


/*	Parse-Funktionen
*/

Obj_t *PFunc_case (io_t *io, void *data)
{
	Obj_t *x;
	int c;

	x = EvalObj(Parse_term(io, OpPrior_Range), NULL);
	c = io_eat(io, " \t");

	if	(x == NULL || c != ':')
	{
		io_error(io, MSG_EFMAIN, 402, 0);
		UnrefObj(x);
		return NULL;
	}

	io_getc(io);
	return Obj_case(new_case(x));
}


Obj_t *PFunc_default (io_t *io, void *data)
{
	int c;

	c = io_eat(io, " \t");

	if	(c != ':')
	{
		io_error(io, MSG_EFMAIN, 403, 0);
		return NULL;
	}

	io_getc(io);
	return Obj_case(new_case(NULL));
}


Obj_t *PFunc_switch (io_t *io, void *data)
{
	Obj_t *var, *body;
	ObjList_t *list, *lp;
	Case_t *buf, **dp;

/*	Schalter bestimmen
*/
	if	(!Parse_fmt(io, "(T)", &var))	return NULL;

/*	Funktionsrumpf abfragen und Befehlsliste extrahieren
*/
	if	(io_eat(io, "%s") != '{')
	{
		UnrefObj(var);
		io_error(io, MSG_EFMAIN, 401, 0);
		return NULL;
	}

	io_getc(io);
	body = Parse_block(io, '}');

	if	(body)
	{
		Block_t *block = body->data;
		list = block->list;
		block->list = NULL;
		UnrefObj(body);
	}
	else	list = NULL;

/*	Eintrittspunkte bestimmen
*/
	if	(!(list && list->obj && list->obj->type == &Type_case))
		io_message(io, MSG_EFMAIN, 404, 0);

	buf = NULL;
	dp = &buf;

	for (lp = list; lp != NULL; lp = lp->next)
	{
		if	(lp->obj && lp->obj->type == &Type_case)
		{
			*dp = Val_case(lp->obj->data);
			Val_case(lp->obj->data) = NULL;
			(*dp)->list = lp->next;
			dp = &(*dp)->next;
		}
	}

	return Obj_call(Expr_switch, NULL,
		MakeObjList(3, var, Obj_case(buf), Obj_list(list)));
}
