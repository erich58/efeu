/*
Blockverarbeitung

$Copyright (C) 1994 Erich Frühstück
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
#include <EFEU/stdtype.h>
#include <EFEU/parsedef.h>
#include <EFEU/cmdeval.h>


/*	Abbruch von Schleifen
*/

static EfiObj *do_break (void *ptr, const EfiObjList *list)
{
	CmdEval_stat = (int) (size_t) ptr;
	return NULL;
}


EfiObj *PFunc_break (IO *io, void *data)
{
	return Obj_call(do_break,
		(void *) (size_t) (data ? CmdEval_Cont : CmdEval_Break), NULL);
}


/*	Beenden eines Funktionsaufrufes
*/

static EfiObj *do_return (void *ptr, const EfiObjList *list)
{
	EfiObj *x;

	UnrefObj(CmdEval_retval);
	CmdEval_retval = NULL;
	x = list ? EvalObj(RefObj(list->obj), NULL) : NULL;
	CmdEval_retval = x;
	CmdEval_stat = CmdEval_Return;
	return NULL;
}


EfiObj *PFunc_return (IO *io, void *data)
{
	return Obj_call(do_return, NULL, NewObjList(Parse_term(io, 0)));
}


/*	Blockstruktur
*/

static EfiObj *e_block (const EfiType *st, const void *ptr);
static void d_block (const EfiType *st, void *tg, int mode);
static void c_block (const EfiType *st, void *tg, const void *source);
EfiType Type_block = EVAL_TYPE("_Block_", EfiBlock, e_block,
	NULL, d_block, c_block);


static void d_block (const EfiType *st, void *tg, int mode)
{
	DelVarTab(((EfiBlock *) tg)->tab);
	DelObjList(((EfiBlock *) tg)->list);
	((EfiBlock *) tg)->tab = NULL;
	((EfiBlock *) tg)->list = NULL;
}


static void c_block (const EfiType *st, void *tg, const void *src)
{
	log_error(NULL, "[efmain:144]", NULL);
}


EfiObj *EvalExpression (const EfiObj *obj)
{
	EfiObj *x;

	UnrefObj(CmdEval_retval);
	CmdEval_retval = NULL;

	x = EvalObj(RefObj(obj), NULL);
	CmdEval_stat = 0;

	if	(x == NULL)
	{
		x = CmdEval_retval;
		CmdEval_retval = NULL;
	}

	return x;
}


static EfiObj *e_block (const EfiType *st, const void *ptr)
{
	EfiObjList *l;

	PushVarTab(RefVarTab(((EfiBlock *) ptr)->tab), NULL);
	PushVarTab(NULL, NULL);
	CmdEval_stat = 0;

	for (l = ((EfiBlock *) ptr)->list; l && !CmdEval_stat; l = l->next)
		UnrefEval(RefObj(l->obj));

	PopVarTab();
	PopVarTab();
	return NULL;
}


/*	Schleifen
*/

EfiObj *Expr_for (void *par, const EfiObjList *list)
{
	EfiObj *test;
	EfiObj *step;
	EfiObj *body;

	CmdEval_stat = 0;
	UnrefEval(RefObj(list->obj));
	list = list->next;

	switch (CmdEval_stat)
	{
	case CmdEval_Break:	CmdEval_stat = 0; /* FALLTHROUGH */
	case CmdEval_Return:	return NULL;
	default:		break;
	}

	test = list->obj;
	step = list->next->obj;
	body = list->next->next->obj;

	for (;;)
	{
		if	(test)
		{
			EfiObj *obj;
			int flag;

			CmdEval_stat = 0;
			obj = EvalObj(RefObj(test), &Type_bool);
			flag = (obj && !Val_bool(obj->data));
			UnrefObj(obj);

			if	(flag)	break;
		}

		CmdEval_stat = 0;
		UnrefEval(RefObj(body));

		if	(CmdEval_stat == CmdEval_Return) return NULL;
		if	(CmdEval_stat == CmdEval_Break)	break;

		UnrefEval(RefObj(step));
	}
	
	CmdEval_stat = 0;
	return NULL;
}


static EfiObj *forin_get (const EfiObj *base, void *data)
{
	return RefObj(Val_obj(data));
}

EfiObj *Expr_forin (void *par, const EfiObjList *list)
{
	VarTabEntry entry;
	EfiObj *body;
	EfiObj *lobj;
	EfiObj *x;

	x = NULL;
	PushVarTab(NULL, NULL);
	entry.name = Val_str(list->obj->data);
	entry.desc = NULL;
	entry.obj = NULL;
	entry.get = forin_get;
	entry.entry_clean = NULL;
	entry.data = &x;
	VarTab_add(NULL, &entry);

	list = list->next;
	lobj = EvalObj(RefObj(list->obj), &Type_list);
	body = list->next->obj;

	if	(lobj == NULL || body == NULL)	return NULL;

	for (list = Val_list(lobj->data); list != NULL; list = list->next)
	{
		x = EvalObj(RefObj(list->obj), NULL);

		if	(x == NULL)	continue;

		CmdEval_stat = 0;
		UnrefEval(RefObj(body));
		UnrefObj(x);

		if	(CmdEval_stat == CmdEval_Return)
			break;

		if	(CmdEval_stat == CmdEval_Break)
		{
			CmdEval_stat = 0;
			break;
		}
		else	CmdEval_stat = 0;
	}

	UnrefObj(lobj);
	PopVarTab();
	return NULL;
}
