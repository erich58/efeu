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

struct CASE {
	EfiObj *obj;
	struct CASE *next;
	EfiObjList *list;
};

static ALLOCTAB(case_tab, "switch-case", 0, sizeof(struct CASE));

static struct CASE *new_case (EfiObj *obj)
{
	struct CASE *data;

	data = new_data(&case_tab);
	data->obj = obj;
	data->next = NULL;
	data->list = NULL;
	return data;
}

static void del_case (struct CASE *ptr)
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

#define	Val_case(x)	((struct CASE **) (x))[0]
#define	Obj_case(x)	NewPtrObj(&Type_case, x)

static EfiObj *eval_case (const EfiType *type, const void *data)
{
	return NULL;
}

static void clean_case (const EfiType *st, void *data, int mode)
{
	del_case(Val_case(data));
	Val_case(data) = NULL;
}

static void copy_case (const EfiType *st, void *tg, const void *src)
{
	Val_case(tg) = new_case(RefObj(Val_case(src)->obj));
}

static EfiType Type_case = EVAL_TYPE("_case_", struct CASE *, eval_case,
	NULL, clean_case, copy_case);


/*	Auswertungsfunktion
*/

static void eval_list (EfiObjList *list)
{
	PushVarTab(NULL, NULL);

	for (CmdEval_stat = 0; list && !CmdEval_stat; list = list->next)
		UnrefEval(RefObj(list->obj));

	if	(CmdEval_stat == CmdEval_Break)
		CmdEval_stat = 0;

	PopVarTab();
}

static EfiObj *Expr_switch (void *par, const EfiObjList *args)
{
	EfiObj *val, *x;
	struct CASE *entry, *ptr, *def;

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

EfiObj *PFunc_case (IO *io, void *data)
{
	EfiObj *x;
	int c;

	x = EvalObj(Parse_term(io, OpPrior_Range), NULL);
	c = io_eat(io, " \t");

	if	(x == NULL || c != ':')
	{
		io_error(io, "[efmain:402]", NULL);
		UnrefObj(x);
		return NULL;
	}

	io_getc(io);
	return Obj_case(new_case(x));
}


EfiObj *PFunc_default (IO *io, void *data)
{
	int c;

	c = io_eat(io, " \t");

	if	(c != ':')
		return NewPtrObj(&Type_name, "default");

	io_getc(io);
	return Obj_case(new_case(NULL));
}


EfiObj *PFunc_switch (IO *io, void *data)
{
	EfiObj *var, *body;
	EfiObjList *list, *lp;
	struct CASE *buf, **dp;

/*	Schalter bestimmen
*/
	if	(!Parse_fmt(io, "(T)", &var))	return NULL;

/*	Funktionsrumpf abfragen und Befehlsliste extrahieren
*/
	if	(io_eat(io, "%s") != '{')
	{
		UnrefObj(var);
		io_error(io, "[efmain:401]", NULL);
		return NULL;
	}

	io_getc(io);
	body = Parse_block(io, '}');

	if	(body)
	{
		EfiBlock *block = body->data;
		list = block->list;
		block->list = NULL;
		UnrefObj(body);
	}
	else	list = NULL;

/*	Eintrittspunkte bestimmen
*/
	if	(!(list && list->obj && list->obj->type == &Type_case))
		io_note(io, "[efmain:404]", NULL);

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
