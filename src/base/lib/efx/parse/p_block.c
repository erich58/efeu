/*	Blockverarbeitung
	(c) 1994 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6

	Version 0.4
*/

#include <EFEU/object.h>
#include <EFEU/stdtype.h>
#include <EFEU/parsedef.h>
#include <EFEU/cmdeval.h>


/*	Abbruch von Schleifen
*/

static Obj_t *do_break(void *ptr, const ObjList_t *list)
{
	CmdEval_stat = (int) (size_t) ptr;
	return NULL;
}


Obj_t *PFunc_break(io_t *io, void *data)
{
	return Obj_call(do_break,
		(void *) (size_t) (data ? CmdEval_Cont : CmdEval_Break), NULL);
}


/*	Beenden eines Funktionsaufrufes
*/

static Obj_t *do_return(void *ptr, const ObjList_t *list)
{
	Obj_t *x;

	UnrefObj(CmdEval_retval);
	x = list ? EvalObj(RefObj(list->obj), NULL) : NULL;

	if	(x && x->lref)
	{
		CmdEval_retval = ConstObj(x->type, x->data);
		UnrefObj(x);
	}
	else	CmdEval_retval = x;

	CmdEval_stat = CmdEval_Return;
	return NULL;
}


Obj_t *PFunc_return(io_t *io, void *data)
{
	return Obj_call(do_return, NULL, NewObjList(Parse_term(io, 0)));
}


/*	Blockstruktur
*/

static Obj_t *e_block (const Type_t *st, const void *ptr);
static void d_block (const Type_t *st, void *tg);
static void c_block (const Type_t *st, void *tg, const void *source);
Type_t Type_block = EVAL_TYPE("_Block_", Block_t, e_block, d_block, c_block);


static void d_block(const Type_t *st, void *tg)
{
	DelVarTab(((Block_t *) tg)->tab);
	DelObjList(((Block_t *) tg)->list);
	((Block_t *) tg)->tab = NULL;
	((Block_t *) tg)->list = NULL;
}


static void c_block(const Type_t *st, void *tg, const void *src)
{
	liberror(MSG_EFMAIN, 144);
}


Obj_t *EvalExpression(const Obj_t *obj)
{
	Obj_t *x;

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


static Obj_t *e_block(const Type_t *st, const void *ptr)
{
	ObjList_t *l;

	PushVarTab(RefVarTab(((Block_t *) ptr)->tab), NULL);
	PushVarTab(NULL, NULL);
	CmdEval_stat = 0;

	for (l = ((Block_t *) ptr)->list; l && !CmdEval_stat; l = l->next)
		UnrefEval(RefObj(l->obj));

	PopVarTab();
	PopVarTab();
	return NULL;
}


/*	Schleifen
*/

Obj_t *Expr_for(void *par, const ObjList_t *list)
{
	Obj_t *test;
	Obj_t *step;
	Obj_t *body;

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
			Obj_t *obj;
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


Obj_t *Expr_forin(void *par, const ObjList_t *list)
{
	Var_t var;
	Obj_t *body;
	Obj_t *lobj;
	Obj_t *x;

	memset(&var, 0, sizeof(Var_t));
	var.name = Val_str(list->obj->data);
	var.dim = 0;
	list = list->next;
	lobj = EvalObj(RefObj(list->obj), &Type_list);
	body = list->next->obj;

	if	(lobj == NULL || body == NULL)	return NULL;

	PushVarTab(NULL, NULL);
	AddVar(NULL, &var, 1);

	for (list = Val_list(lobj->data); list != NULL; list = list->next)
	{
		x = EvalObj(RefObj(list->obj), NULL);

		if	(x == NULL)	continue;

		var.type = x->type;
		var.data = x->data;
		CmdEval_stat = 0;
		UnrefEval(RefObj(body));
		var.type = NULL;
		var.data = NULL;
		UnrefObj(x);

		if	(CmdEval_stat == CmdEval_Return) return NULL;
		if	(CmdEval_stat == CmdEval_Break) break;
	}

	CmdEval_stat = 0;
	UnrefObj(lobj);
	PopVarTab();
	return NULL;
}
