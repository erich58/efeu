/*
Standardoperatoren

$Copyright (C) 1994, 2002 Erich Frühstück
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
#include <EFEU/konvobj.h>
#include <EFEU/Debug.h>
#include <EFEU/Op.h>

#define	E70	"[efmain:70]$!: operator $1 not found.\n"
#define	E101	"[efmain:101]$!: $0: operator $1: right argument is missing.\n"
#define	E102	"[efmain:102]$!: $0: conditional expression: ':' is missing.\n"
#define	E103	"[efmain:103]$!: $0: partial term: missing right parenthese.\n"
#define	E104	"[efmain:104]$!: $0: expression: missing right bracket.\n"
#define	E194	"[efmain:194]$!: $0: syntax error in vector index.\n"
#define	E196	"[efmain:196]$!: $0: left operand is not an lvalue.\n"
#define E214	"[efmain:214]$!: argument of operator $1 is not defined.\n"
#define E215	"[efmain:215]$!: operator $1 for type $2 is not defined.\n"

static EfiObj *objlist (IO *io, EfiOp *op, EfiObj *left);
static EfiObj *globvar (IO *io, EfiOp *op, EfiObj *left);
static EfiObj *get_right(IO *io, const EfiOp *op);
static EfiObj *conditional (IO *io, EfiOp *op, EfiObj *left);
static EfiObj *subterm (IO *io, EfiOp *op, EfiObj *left);
static EfiObj *expression (IO *io, EfiOp *op, EfiObj *left);
static EfiObj *funcarg (IO *io, EfiOp *op, EfiObj *left);
static EfiObj *vecindex(IO *io, EfiOp *op, EfiObj *left);
static EfiObj *member_op (IO *io, EfiOp *op, EfiObj *left);
static EfiObj *scope_op (IO *io, EfiOp *op, EfiObj *left);
static EfiObj *bool_op (IO *io, EfiOp *op, EfiObj *left);
static EfiObj *comma_op (IO *io, EfiOp *op, EfiObj *left);
static EfiObj *range_op (IO *io, EfiOp *op, EfiObj *left);
static EfiObj *assign_op (IO *io, EfiOp *op, EfiObj *left);
static EfiObj *left_assign (IO *io, EfiOp *op, EfiObj *left);

static EfiObj *bool_and (void *par, const EfiObjList *list);
static EfiObj *bool_or (void *par, const EfiObjList *list);
static EfiObj *cast_expr (void *par, const EfiObjList *list);
static EfiObj *lcast_expr (void *par, const EfiObjList *list);
static EfiObj *cond_expr (void *par, const EfiObjList *list);

void AddOpDef(NameKeyTab *tab, EfiOp *def, size_t dim)
{
	for (; dim-- > 0; def++)
		nkt_insert(tab, def->name, def);
}


/*	Linke Operatoren
*/

static EfiOp std_prefix[] = {
	{ "(", OpPrior_Unary, OpAssoc_Right, subterm },
	{ "+", OpPrior_Unary, OpAssoc_Right, PrefixOp },
	{ "-", OpPrior_Unary, OpAssoc_Right, PrefixOp },
	{ "!", OpPrior_Unary, OpAssoc_Right, PrefixOp },
	{ "~", OpPrior_Unary, OpAssoc_Right, PrefixOp },
	{ "::", OpPrior_Unary, OpAssoc_Right, globvar },
	{ "{", OpPrior_Unary, OpAssoc_Right, objlist },
	{ "[", OpPrior_Unary, OpAssoc_Right, expression },
	{ "--", OpPrior_Unary, OpAssoc_Right, left_assign },
	{ "++", OpPrior_Unary, OpAssoc_Right, left_assign },
};


/*	Binäre Operatoren
*/

static EfiOp std_postfix[] = {
	{ "::", OpPrior_Scope, OpAssoc_Left, scope_op },
	{ ".", OpPrior_Member, OpAssoc_Left, member_op },
	{ "(", OpPrior_Unary, OpAssoc_Right, funcarg },
	{ "[", OpPrior_Unary, OpAssoc_Right, vecindex },
	{ "++", OpPrior_Unary, OpAssoc_Right, PostfixOp },
	{ "--", OpPrior_Unary, OpAssoc_Right, PostfixOp },

	{ "*", OpPrior_Mult, OpAssoc_Left, BinaryOp },
	{ "/", OpPrior_Mult, OpAssoc_Left, BinaryOp },
	{ "%", OpPrior_Mult, OpAssoc_Left, BinaryOp },

	{ "+", OpPrior_Add, OpAssoc_Left, BinaryOp },
	{ "-", OpPrior_Add, OpAssoc_Left, BinaryOp },

	{ "<<", OpPrior_Shift, OpAssoc_Left, BinaryOp },
	{ ">>", OpPrior_Shift, OpAssoc_Left, BinaryOp },

	{ "<", OpPrior_Compare, OpAssoc_Left, BinaryOp },
	{ "<=", OpPrior_Compare, OpAssoc_Left, BinaryOp },
	{ ">", OpPrior_Compare, OpAssoc_Left, BinaryOp },
	{ ">=", OpPrior_Compare, OpAssoc_Left, BinaryOp },

	{ "==", OpPrior_Equal, OpAssoc_Left, BinaryOp },
	{ "!=", OpPrior_Equal, OpAssoc_Left, BinaryOp },

	{ "&", OpPrior_BitAnd, OpAssoc_Left, BinaryOp },
	{ "^", OpPrior_BitXor, OpAssoc_Left, BinaryOp },
	{ "|", OpPrior_BitOr, OpAssoc_Left, BinaryOp },

	{ "&&", OpPrior_And, OpAssoc_Left, bool_op },
	{ "||", OpPrior_Or, OpAssoc_Left, bool_op },

	{ "?", OpPrior_Cond, OpAssoc_Right, conditional },

	{ ":=", OpPrior_Assign, OpAssoc_Right, AssignOp },
	{ "*=", OpPrior_Assign, OpAssoc_Right, AssignOp },
	{ "/=", OpPrior_Assign, OpAssoc_Right, AssignOp },
	{ "%=", OpPrior_Assign, OpAssoc_Right, AssignOp },
	{ "+=", OpPrior_Assign, OpAssoc_Right, AssignOp },
	{ "-=", OpPrior_Assign, OpAssoc_Right, AssignOp },
	{ "<<=", OpPrior_Assign, OpAssoc_Right, AssignOp },
	{ ">>=", OpPrior_Assign, OpAssoc_Right, AssignOp },
	{ "&=", OpPrior_Assign, OpAssoc_Right, AssignOp },
	{ "^=", OpPrior_Assign, OpAssoc_Right, AssignOp },
	{ "|=", OpPrior_Assign, OpAssoc_Right, AssignOp },
	
	{ "=", OpPrior_Assign, OpAssoc_Right, assign_op },
	{ ":", OpPrior_Range, OpAssoc_Left, range_op },
	{ ",", OpPrior_Comma, OpAssoc_Left, comma_op },
};


NameKeyTab PrefixTab = NKT_DATA("Prefix", 64, NULL);
NameKeyTab PostfixTab = NKT_DATA("Postfix", 64, NULL);


void StdOpDef(void)
{
	AddOpDef(&PrefixTab, std_prefix, tabsize(std_prefix));
	AddOpDef(&PostfixTab, std_postfix, tabsize(std_postfix));
}


static EfiObj *get_right(IO *io, const EfiOp *op)
{
	EfiObj *right;

	right = Parse_term(io, op->prior);

	if	(right == NULL)
		io_error(io, E101, "s", op->name);

	return right;
}


static EfiObj *conditional(IO *io, EfiOp *op, EfiObj *left)
{
	EfiObj *a, *b;

	a = Parse_term(io, op->prior);

	if	(io_eat(io, " \t") == ':')
	{
		io_getc(io);
		b = Parse_term(io, op->prior);
	}
	else
	{
		io_error(io, E102, NULL);
		b = NULL;
	}

	return CondTerm(left, a, b);
}

static EfiObj *range_op(IO *io, EfiOp *op, EfiObj *obj)
{
	EfiObjList *list;
	void *func;
	int vflag;

	vflag = obj->type->eval ? 1 : 0;
	list = NewObjList(obj);
	obj = Parse_term(io, op->prior);
	list->next = NewObjList(obj);

	if	(obj && !obj->type->eval)
		vflag = 0;

	if	(io_eat(io, " \t") == ':')
	{
		io_getc(io);
		obj = Parse_term(io, op->prior);
		list->next->next = NewObjList(obj);

		if	(obj && !obj->type->eval)
			vflag = 0;
	}

	func = GetGlobalFunc(op->name);
	
	if	(func == NULL)
	{
		dbg_note(NULL, E70, "s", op->name);
		DelObjList(list);
		return NULL;
	}

	if	(vflag)
		return Obj_call(Expr_func, func, list);

	obj = EvalFunc(func, list);
	DelObjList(list);
	return obj;
}

static EfiObj *subterm(IO *io, EfiOp *op, EfiObj *left)
{
	EfiObj *obj = Parse_term(io, 0);

	if	(io_eat(io, " \t") != ')')
	{
		io_error(io, E103, NULL);
	}
	else	io_getc(io);

	if	(!obj)	return NULL;

	if	(obj->type == &Type_type)
	{
		EfiType *type = Val_type(obj->data);

		UnrefObj(obj);
		obj = Parse_term(io, OpPrior_Unary);

		if	(obj && obj->type->eval)
		{
			obj = Obj_call(cast_expr, type, NewObjList(obj));
		}
		else	obj = EvalObj(obj, type);
	}
	else if	(obj->type == &Type_lval)
	{
		EfiType *type = Val_type(obj->data);

		UnrefObj(obj);
		obj = Parse_term(io, OpPrior_Unary);
		obj = Obj_call(lcast_expr, type, NewObjList(obj));
	}

	return obj;
}

static EfiObj *globvar(IO *io, EfiOp *op, EfiObj *left)
{
	char *p;

	if	((p = Parse_name(io)) != NULL)
		return GetVar(GlobalVar, p, NULL);

	io_error(io, E196, NULL);
	return NULL;
}

static EfiObj *bool_and(void *par, const EfiObjList *list)
{
	EfiObj *obj;

	while (list != NULL)
	{
		obj = EvalObj(RefObj(list->obj), &Type_bool);

		if	(!(obj && Val_bool(obj->data)))
			return bool2Obj(0);

		list = list->next;
	}

	return bool2Obj(1);
}


static EfiObj *bool_or(void *par, const EfiObjList *list)
{
	EfiObj *obj;

	while (list != NULL)
	{
		obj = EvalObj(RefObj(list->obj), &Type_bool);

		if	(obj && Val_bool(obj->data))
			return bool2Obj(1);

		list = list->next;
	}

	return bool2Obj(0);
}


static EfiObj *bool_op(IO *io, EfiOp *op, EfiObj *left)
{
	EfiObj *(*func) (void *par, const EfiObjList *list);
	EfiObjList *list;
	EfiObj *right;

	if	(!(right = get_right(io, op)))
		return left;

	func = (op->prior == OpPrior_And) ? bool_and : bool_or;
	list = NewObjList(left);
	list->next = NewObjList(right);

	if	(!left->type->eval && !right->type->eval)
	{
		left = func(NULL, list);
		DelObjList(list);
	}
	else	left = Obj_call(func, NULL, list);

	return left;
}


static EfiObj *funcarg(IO *io, EfiOp *op, EfiObj *left)
{
	EfiObjList *list;
	EfiVirFunc *virfunc;

	list = Parse_list(io, ')');
	virfunc = NULL;

	if	(left->type == &Type_type)
	{
		EfiType *type = Val_type(left->data);
		virfunc = type ? type->create : NULL;
	}

	if	(virfunc != NULL)
	{
		UnrefObj(left);
		return Obj_call(Expr_virfunc, virfunc, list);
	}
	else	return BinaryTerm("()", left, Obj_list(list));
}


static EfiObj *name_obj (IO *io, EfiType *type, EfiObj *obj)
{
	EfiName name;
	name.obj = obj;
	name.name = NULL;

	if	((name.name = Parse_name(io)) != NULL)
		return NewObj(type, &name);

	UnrefObj(obj);
	return NULL;
}

static EfiObj *member_op (IO *io, EfiOp *op, EfiObj *left)
{
	return name_obj(io, &Type_mname, left);
}


static EfiObj *scope_op(IO *io, EfiOp *op, EfiObj *left)
{
	return name_obj(io, &Type_sname, left);
}


static EfiObj *cond_expr(void *par, const EfiObjList *list)
{
	EfiObj *obj;

	obj = EvalObj(RefObj(list->obj), &Type_bool);

	if	(!obj || !Val_bool(obj->data))
		list = list->next;

	UnrefObj(obj);
	return RefObj(list->next->obj);
}


EfiObj *CondTerm(EfiObj *test, EfiObj *ifpart, EfiObj *elsepart)
{
	if	(test == NULL || test->type == NULL)
	{
		UnrefObj(test);
		UnrefObj(ifpart);
		return elsepart;
	}

	if	(test->type->eval != NULL)
	{
		EfiObjList *list;

		list = NewObjList(test);
		list->next = NewObjList(ifpart);
		list->next->next = NewObjList(elsepart);
		return Obj_call(cond_expr, NULL, list);
	}

	test = EvalObj(test, &Type_bool);

	if	(!test || !Val_bool(test->data))
	{
		UnrefObj(ifpart);
		ifpart = elsepart;
	}
	else	UnrefObj(elsepart);

	UnrefObj(test);
	return ifpart;
}

static EfiObj *cast_expr(void *par, const EfiObjList *list)
{
	EfiObj *obj = NewObj(par, NULL);
	Obj2Data(RefObj(list->obj), obj->type, obj->data);
	return obj;
}

static EfiObj *lcast_expr(void *par, const EfiObjList *list)
{
	return KonvLval(RefObj(list->obj), par);
}

/*	Komma - Operator
*/

static EfiObj *comma_op(IO *io, EfiOp *op, EfiObj *left)
{
#if	COMMA_LIST
	EfiObjList *list = NewObjList(left);

	list->next = Parse_list(io, EOF);
	return NewPtrObj(&Type_explist, list);
#else
	return CommaTerm(left, get_right(io, op));
#endif
}


#if	COMMA_LIST

EfiObj *CommaTerm(EfiObj *left, EfiObj *right)
{
	if	(left == NULL)
		return right;

	if	(left->type != &Type_explist)
		left = NewPtrObj(&Type_explist, NewObjList(left));

	ExpandObjList(left->data, right);
	return left;
}

#else

static EfiObj *comma_expr(void *par, const EfiObjList *list)
{
	UnrefEval(RefObj(list->obj));
	return EvalObj(RefObj(list->next->obj), NULL);
}

EfiObj *CommaTerm(EfiObj *left, EfiObj *right)
{
	if	(left && left->type->eval)
	{
		EfiObjList *list;

		list = NewObjList(left);
		list->next = NewObjList(right);
		return Obj_call(comma_expr, NULL, list);
	}

	UnrefEval(left);
	return right;
}

#endif

/*	Zuweisungsoperator
*/

static EfiObj *Assign_expr(void *par, const EfiObjList *list)
{
	EfiObj *left;

	left = EvalObj(RefObj(list->obj), NULL);
	return AssignTerm(par, left, RefObj(list->next->obj));
}

EfiObj *AssignOp(IO *io, EfiOp *op, EfiObj *left)
{
	EfiObj *right;
	EfiObjList *list;

	if	(left == NULL)	return NULL;

	right = get_right(io, op);

	if	(right == NULL)	return left;

	list = NewObjList(left);
	list->next = NewObjList(right);
	return Obj_call(Assign_expr, (void *) op->name, list);
}


/*	Einfache Zuweisung
*/

static EfiObj *assign_expr(void *par, const EfiObjList *list)
{
	EfiObj *left;

	left = EvalObj(RefObj(list->obj), NULL);
	return AssignObj(left, RefObj(list->next->obj));
}


static EfiObj *assign_op(IO *io, EfiOp *op, EfiObj *left)
{
	EfiObj *right;
	EfiObjList *list;

	if	(left == NULL)	return NULL;

	right = get_right(io, op);

	if	(right == NULL)	return left;

	list = NewObjList(left);
	list->next = NewObjList(right);
	return Obj_call(assign_expr, (void *) op->name, list);
}


EfiObj *Parse_index(IO *io)
{
	EfiObj *idx;
	int c;

	idx = Parse_term(io, 0);

	if	((c = io_eat(io, " \t")) != ']')
	{
		io_error(io, E194, NULL);
	}
	else	io_getc(io);

	return idx;
}

static EfiObj *vecindex(IO *io, EfiOp *op, EfiObj *left)
{
	return BinaryTerm("[]", left, Parse_index(io));
}


static EfiObj *objlist(IO *io, EfiOp *op, EfiObj *left)
{
	return NewPtrObj(&Type_explist, Parse_list(io, '}'));
}

static EfiObj *expression(IO *io, EfiOp *op, EfiObj *left)
{
	left = expr2Obj(Parse_cmd(io));

	if	(io_eat(io, " \t") != ']')
	{
		io_error(io, E104, NULL);
	}
	else	io_getc(io);

	return left;
}

static EfiObj *LeftAssign (const char *name, EfiObj *obj)
{
	EfiObjList *list;
	void *func;

	if	(obj == NULL)
	{
		dbg_note(NULL, E214, "s", name);
		return NULL;
	}

	if	((func = GetTypeFunc(obj->type, name)) == NULL)
	{
		dbg_note(NULL, E215, "ss", name, obj->type->name);
		UnrefObj(obj);
		return NULL;
	}

	list = NewObjList(obj);
	obj = EvalFunc(func, list);
	DelObjList(list);
	return obj;
}

static EfiObj *lassign_expr(void *par, const EfiObjList *list)
{
	return LeftAssign(par, EvalObj(RefObj(list->obj), NULL));
}

static EfiObj *left_assign(IO *io, EfiOp *op, EfiObj *left)
{
	if	((left = Parse_term(io, op->prior)) == NULL)
	{
		io_error(io, E101, "s", op->name);
		return NULL;
	}

	if	(left->type->eval)
		return Obj_call(lassign_expr, op->name, NewObjList(left));

	return LeftAssign(op->name, left);
}
