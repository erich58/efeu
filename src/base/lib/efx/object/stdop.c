/*
Standardoperatoren

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
#include <EFEU/konvobj.h>
#include <EFEU/Op.h>

static int opcmp (const Op_t *a, const Op_t *b);
static Obj_t *objlist (io_t *io, Op_t *op, Obj_t *left);
static Obj_t *globvar (io_t *io, Op_t *op, Obj_t *left);
static Obj_t *get_right(io_t *io, const Op_t *op);
static Obj_t *conditional (io_t *io, Op_t *op, Obj_t *left);
static Obj_t *subterm (io_t *io, Op_t *op, Obj_t *left);
static Obj_t *funcarg (io_t *io, Op_t *op, Obj_t *left);
static Obj_t *vecindex(io_t *io, Op_t *op, Obj_t *left);
static Obj_t *member_op (io_t *io, Op_t *op, Obj_t *left);
static Obj_t *scope_op (io_t *io, Op_t *op, Obj_t *left);
static Obj_t *bool_op (io_t *io, Op_t *op, Obj_t *left);
static Obj_t *comma_op (io_t *io, Op_t *op, Obj_t *left);
static Obj_t *range_op (io_t *io, Op_t *op, Obj_t *left);
static Obj_t *assign_op (io_t *io, Op_t *op, Obj_t *left);
static Obj_t *left_assign (io_t *io, Op_t *op, Obj_t *left);

static Obj_t *bool_and (void *par, const ObjList_t *list);
static Obj_t *bool_or (void *par, const ObjList_t *list);
static Obj_t *cast_expr (void *par, const ObjList_t *list);
static Obj_t *cond_expr (void *par, const ObjList_t *list);

static int opcmp(const Op_t *a, const Op_t *b)
{
	return mstrcmp(b->name, a->name);
}

void AddOpDef(xtab_t *tab, Op_t *def, size_t dim)
{
	xappend(tab, def, dim, sizeof(Op_t), XS_REPLACE);
}


/*	Linke Operatoren
*/

static Op_t std_prefix[] = {
	{ "(", OpPrior_Unary, OpAssoc_Right, subterm },
	{ "+", OpPrior_Unary, OpAssoc_Right, PrefixOp },
	{ "-", OpPrior_Unary, OpAssoc_Right, PrefixOp },
	{ "!", OpPrior_Unary, OpAssoc_Right, PrefixOp },
	{ "#", OpPrior_Unary, OpAssoc_Right, PrefixOp },
	{ "~", OpPrior_Unary, OpAssoc_Right, PrefixOp },
	{ "::", OpPrior_Unary, OpAssoc_Right, globvar },
	{ "{", OpPrior_Unary, OpAssoc_Right, objlist },
	{ "--", OpPrior_Unary, OpAssoc_Right, left_assign },
	{ "++", OpPrior_Unary, OpAssoc_Right, left_assign },
};


/*	Binäre Operatoren
*/

static Op_t std_postfix[] = {
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


XTAB(PrefixTab, 0, (comp_t) opcmp);
XTAB(PostfixTab, 0, (comp_t) opcmp);


void StdOpDef(void)
{
	AddOpDef(&PrefixTab, std_prefix, tabsize(std_prefix));
	AddOpDef(&PostfixTab, std_postfix, tabsize(std_postfix));
}


static Obj_t *get_right(io_t *io, const Op_t *op)
{
	Obj_t *right;

	right = Parse_term(io, op->prior);

	if	(right == NULL)
		io_error(io, MSG_EFMAIN, 101, 1, op->name);

	return right;
}


static Obj_t *conditional(io_t *io, Op_t *op, Obj_t *left)
{
	Obj_t *a, *b;

	a = Parse_term(io, op->prior);

	if	(io_eat(io, " \t") == ':')
	{
		io_getc(io);
		b = Parse_term(io, op->prior);
	}
	else
	{
		io_error(io, MSG_EFMAIN, 102, 0);
		b = NULL;
	}

	return CondTerm(left, a, b);
}

static Obj_t *range_op(io_t *io, Op_t *op, Obj_t *left)
{
	Obj_t *a, *b;

	a = Parse_term(io, op->prior);

	if	(io_eat(io, " \t") == ':')
	{
		io_getc(io);
		b = Parse_term(io, op->prior);
	}
	else	b = NULL;

	return RangeTerm(left, a, b);
}

static Obj_t *subterm(io_t *io, Op_t *op, Obj_t *left)
{
	Obj_t *obj;

	obj = Parse_term(io, 0);

	if	(io_eat(io, " \t") != ')')
	{
		io_error(io, MSG_EFMAIN, 103, 0);
	}
	else	io_getc(io);

	if	(obj && obj->type == &Type_type)
	{
		Obj_t *x;

		x = Parse_term(io, OpPrior_Unary);

		if	(x && x->type->eval)
		{
			ObjList_t *list;

			list = NewObjList(obj);
			list->next = NewObjList(x);
			x = Obj_call(cast_expr, NULL, list);
		}
		else
		{
			x = EvalObj(x, Val_type(obj->data));
			UnrefObj(obj);
		}

		return x;
	}

	return obj;
}

static Obj_t *globvar(io_t *io, Op_t *op, Obj_t *left)
{
	char *p;

	if	((p = Parse_name(io)) != NULL)
		return GetVar(GlobalVar, p, NULL);

	io_error(io, MSG_EFMAIN, 196, 0);
	return NULL;
}

static Obj_t *bool_and(void *par, const ObjList_t *list)
{
	Obj_t *obj;

	while (list != NULL)
	{
		obj = EvalObj(RefObj(list->obj), &Type_bool);

		if	(!(obj && Val_bool(obj->data)))
			return bool2Obj(0);

		list = list->next;
	}

	return bool2Obj(1);
}


static Obj_t *bool_or(void *par, const ObjList_t *list)
{
	Obj_t *obj;

	while (list != NULL)
	{
		obj = EvalObj(RefObj(list->obj), &Type_bool);

		if	(obj && Val_bool(obj->data))
			return bool2Obj(1);

		list = list->next;
	}

	return bool2Obj(0);
}


static Obj_t *bool_op(io_t *io, Op_t *op, Obj_t *left)
{
	EvalExpr_t func;
	ObjList_t *list;
	Obj_t *right;

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


static Obj_t *funcarg(io_t *io, Op_t *op, Obj_t *left)
{
	ObjList_t *list;
	VirFunc_t *virfunc;

	list = Parse_list(io, ')');
	virfunc = NULL;

	if	(left->type == &Type_type)
	{
		Type_t *type = Val_type(left->data);
		virfunc = type ? type->create : NULL;
	}

	if	(virfunc != NULL)
	{
		UnrefObj(left);
		return Obj_call(Expr_virfunc, virfunc, list);
	}
	else	return BinaryTerm("()", left, Obj_list(list));
}


static Obj_t *name_obj (io_t *io, Type_t *type, Obj_t *obj)
{
	Name_t name;
	name.obj = obj;
	name.name = NULL;

	if	((name.name = Parse_name(io)) != NULL)
		return NewObj(type, &name);

	UnrefObj(obj);
	return NULL;
}

static Obj_t *member_op (io_t *io, Op_t *op, Obj_t *left)
{
	return name_obj(io, &Type_mname, left);
}


static Obj_t *scope_op(io_t *io, Op_t *op, Obj_t *left)
{
	return name_obj(io, &Type_sname, left);
}


static Obj_t *cond_expr(void *par, const ObjList_t *list)
{
	Obj_t *obj;

	obj = EvalObj(RefObj(list->obj), &Type_bool);

	if	(!obj || !Val_bool(obj->data))
		list = list->next;

	UnrefObj(obj);
	return RefObj(list->next->obj);
}


Obj_t *CondTerm(Obj_t *test, Obj_t *ifpart, Obj_t *elsepart)
{
	if	(test == NULL || test->type == NULL)
	{
		UnrefObj(test);
		UnrefObj(ifpart);
		return elsepart;
	}

	if	(test->type->eval != NULL)
	{
		ObjList_t *list;

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

static Obj_t *make_range(Obj_t *first, Obj_t *last, Obj_t *step)
{
	ArgKonv_t *konv;
	FuncArg_t *arg;
	ObjList_t *list, **ptr;
	Type_t *type;
	Func_t *f_cmp;
	Func_t *f_step;
	int c, x;

	if	(first == NULL)
	{
		UnrefObj(last);
		UnrefObj(step);
		return NewPtrObj(&Type_list, NULL);
	}

	if	(last == NULL)
	{
		UnrefObj(step);
		return NewPtrObj(&Type_list, NewObjList(first));
	}

/*	Additionsfunktion bestimmen
*/
	arg = FuncArg(2, first->type, 0, last->type, 0);
	f_cmp = SearchFunc(GetGlobalFunc("cmp"), arg, 2, &konv);
	memfree(arg);

	if	(f_cmp == NULL)
	{
		errmsg(MSG_EFMAIN, 216);
		UnrefObj(step);
		return NewPtrObj(&Type_list, MakeObjList(2, first, last));
	}

	if	(konv)
	{
		first = EvalObj(first, f_cmp->arg[0].type);
		last = EvalObj(last, f_cmp->arg[1].type);
		memfree(konv);
	}

	CallFunc(&Type_int, &c, f_cmp, first->data, last->data);

	if	(c == 0)
	{
		UnrefObj(last);
		UnrefObj(step);
		return NewPtrObj(&Type_list, NewObjList(first));
	}

	if	(step == NULL)
		step = int2Obj(c < 0 ? 1 : -1);

	type = first->type;
	f_step = GetFunc(NULL, GetTypeFunc(type, "+="), 2,
		type, 1, step->type, 0); 

	if	(f_step == NULL)
	{
		errmsg(MSG_EFMAIN, 217);
		UnrefObj(step);
		return NewPtrObj(&Type_list, MakeObjList(2, first, last));
	}

	if	(first->lval)
	{
		Obj_t *x = ConstObj(type, first->data);
		UnrefObj(first);
		first = x;
	}

	list = NULL;
	ptr = &list;
	x = c;

	do
	{
		*ptr = NewObjList(ConstObj(type, first->data));
		ptr = &(*ptr)->next;
		CallVoidFunc(f_step, first->data, step->data);
		CallFunc(&Type_int, &c, f_cmp, first->data, last->data);
	}
	while (c * x >= 0);

	UnrefObj(first);
	UnrefObj(last);
	UnrefObj(step);
	return NewPtrObj(&Type_list, list);
}

#define	LOBJ(x)	EvalObj(RefObj((x)->obj), NULL)

static Obj_t *range_expr(void *par, const ObjList_t *list)
{
	if	(list == NULL || list->next == NULL)
		return NewPtrObj(&Type_explist, list);

	return make_range(LOBJ(list), LOBJ(list->next),
		list->next->next ? LOBJ(list->next->next) : NULL);
}

#define	VAR(x)	((x) && (x)->type->eval)

Obj_t *RangeTerm(Obj_t *first, Obj_t *last, Obj_t *step)
{
	if	(VAR(first) || VAR(last) || VAR(step))
	{
		ObjList_t *list = NewObjList(first);
		list->next = NewObjList(last);
		list->next->next = step ? NewObjList(step) : NULL;
		return Obj_call(range_expr, NULL, list);
	}
	else	return make_range(first, last, step);
}

static Obj_t *cast_expr(void *par, const ObjList_t *list)
{
	return EvalObj(RefObj(list->next->obj), Val_type(list->obj->data));
}


/*	Komma - Operator
*/

static Obj_t *comma_op(io_t *io, Op_t *op, Obj_t *left)
{
#if	COMMA_LIST
	ObjList_t *list = NewObjList(left);

	list->next = Parse_list(io, EOF);
	return NewPtrObj(&Type_explist, list);
#else
	return CommaTerm(left, get_right(io, op));
#endif
}


#if	COMMA_LIST

Obj_t *CommaTerm(Obj_t *left, Obj_t *right)
{
	if	(left == NULL)
		return right;

	if	(left->type != &Type_explist)
		left = NewPtrObj(&Type_explist, NewObjList(left));

	ExpandObjList(left->data, right);
	return left;
}

#else

static Obj_t *comma_expr(void *par, const ObjList_t *list)
{
	UnrefEval(RefObj(list->obj));
	return EvalObj(RefObj(list->next->obj), NULL);
}

Obj_t *CommaTerm(Obj_t *left, Obj_t *right)
{
	if	(left && left->type->eval)
	{
		ObjList_t *list;

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

static Obj_t *Assign_expr(void *par, const ObjList_t *list)
{
	Obj_t *left;

	left = EvalObj(RefObj(list->obj), NULL);
	return AssignTerm(par, left, RefObj(list->next->obj));
}

Obj_t *AssignOp(io_t *io, Op_t *op, Obj_t *left)
{
	Obj_t *right;
	ObjList_t *list;

	if	(left == NULL)	return NULL;

	right = get_right(io, op);

	if	(right == NULL)	return left;

	list = NewObjList(left);
	list->next = NewObjList(right);
	return Obj_call(Assign_expr, (void *) op->name, list);
}


/*	Einfache Zuweisung
*/

static Obj_t *assign_expr(void *par, const ObjList_t *list)
{
	Obj_t *left;

	left = EvalObj(RefObj(list->obj), NULL);
	return AssignObj(left, RefObj(list->next->obj));
}


static Obj_t *assign_op(io_t *io, Op_t *op, Obj_t *left)
{
	Obj_t *right;
	ObjList_t *list;

	if	(left == NULL)	return NULL;

	right = get_right(io, op);

	if	(right == NULL)	return left;

	list = NewObjList(left);
	list->next = NewObjList(right);
	return Obj_call(assign_expr, (void *) op->name, list);
}


Obj_t *Parse_index(io_t *io)
{
	Obj_t *idx;
	int c;

	idx = Parse_term(io, 0);

	if	((c = io_eat(io, " \t")) != ']')
	{
		io_error(io, MSG_EFMAIN, 194, 0);
	}
	else	io_getc(io);

	return idx;
}

static Obj_t *vecindex(io_t *io, Op_t *op, Obj_t *left)
{
	return BinaryTerm("[]", left, Parse_index(io));
}


static Obj_t *objlist(io_t *io, Op_t *op, Obj_t *left)
{
	return NewPtrObj(&Type_explist, Parse_list(io, '}'));
}

static Obj_t *LeftAssign (const char *name, Obj_t *obj)
{
	ObjList_t *list;
	void *func;

	if	(obj == NULL)
	{
		reg_cpy(1, name);
		errmsg(MSG_EFMAIN, 214);
		return NULL;
	}

	if	((func = GetTypeFunc(obj->type, name)) == NULL)
	{
		reg_cpy(1, name);
		reg_cpy(2, obj->type->name);
		errmsg(MSG_EFMAIN, 215);
		UnrefObj(obj);
		return NULL;
	}

	list = NewObjList(obj);
	obj = EvalFunc(func, list);
	DelObjList(list);
	return obj;
}

static Obj_t *lassign_expr(void *par, const ObjList_t *list)
{
	return LeftAssign(par, EvalObj(RefObj(list->obj), NULL));
}

static Obj_t *left_assign(io_t *io, Op_t *op, Obj_t *left)
{
	if	((left = Parse_term(io, op->prior)) == NULL)
	{
		io_error(io, MSG_EFMAIN, 101, 1, op->name);
		return NULL;
	}

	if	(left->type->eval)
		return Obj_call(lassign_expr, op->name, NewObjList(left));

	return LeftAssign(op->name, left);
}
