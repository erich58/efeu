/*
Allgemeine Funktionen

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
#include <EFEU/Op.h>
#include <EFEU/cmdconfig.h>

#define	CMPFUNC(name,op,cmp)	\
static void name (EfiFunc *func, void *rval, void **arg) \
{ int x; Val_int(rval) = cmp(&x, arg) && (x op 0) ? 1 : 0; }


/*	Ausdrücke auswerten
*/

static void f_str2expr(EfiFunc *func, void *rval, void **arg)
{
	IO *io;

	io = io_cstr(Val_str(arg[0]));
	Val_obj(rval) = Parse_cmd(io);
	io_close(io);
}

CEXPR(f_str2virfunc, Val_vfunc(rval) = VirFunc(GetGlobalFunc(Val_str(arg[0]))))
CEXPR(f_xeval, Val_obj(rval) = EvalExpression(Val_obj(arg[0])))

static void f_xeval2 (EfiFunc *func, void *rval, void **arg)
{
	PushVarTab(RefVarTab(Val_vtab(arg[1])), NULL);
	Val_obj(rval) = EvalExpression(Val_obj(arg[0]));
	PopVarTab();
}

/*	Dimensionen und Indizes
*/

CEXPR(l_dim, Val_int(rval) = ObjListLen(Val_list(arg[0])))
CEXPR(v_dim, Val_int(rval) = ((EfiVec *) arg[0])[0].dim)
CEXPR(c_dim, Val_int(rval) = 1)

CEXPR(v_index, Val_obj(rval) = Vector(arg[0], Val_int(arg[1])))

static void v_list(EfiFunc *func, void *rval, void **arg)
{
	EfiObjList *base, *list, **ptr;

	base = Val_list(arg[1]);
	list = NULL;
	ptr = &list;

	while (base != NULL)
	{
		*ptr = NewObjList(Vector(arg[0], Obj2int(RefObj(base->obj))));
		ptr = &(*ptr)->next;
		base = base->next;
	}

	Val_list(rval) = list;
}

/*	Zuweisungsoperatoren für allgemeine Objekte
*/

static void base_assign(EfiFunc *func, void *rval, void **arg)
{
	Val_obj(rval) = AssignTerm("=", RefObj(arg[0]), RefObj(arg[1]));
}

static void assign_op(EfiFunc *func, void *rval, void **arg)
{
	EfiObj *left, *right;
	char *p;

	left = RefObj(arg[0]);
	p = mstrncpy(func->name, strlen(func->name) - 1);
	right = BinaryTerm(p, RefObj(left), RefObj(arg[1]));
	memfree(p);
	Val_obj(rval) = AssignObj(left, right);
}

static int do_cmp(int *val, void **arg)
{
	EfiObj *x = EvalObj(BinaryTerm("cmp", RefObj(arg[0]),
		RefObj(arg[1])), &Type_int);

	if	(x)
	{
		*val = Obj2int(x);
		return 1;
	}

	*val = 0;
	return 0;
}

CMPFUNC(any_lt,<,do_cmp)
CMPFUNC(any_le,<=,do_cmp)
CMPFUNC(any_eq,==,do_cmp)
CMPFUNC(any_ne,!=,do_cmp)
CMPFUNC(any_ge,>=,do_cmp)
CMPFUNC(any_gt,>,do_cmp)

/*	Increment- und Dekrementoperatoren
*/

static void f_pre_inc(EfiFunc *func, void *rval, void **arg)
{
	Val_obj(rval) = EvalObj(AssignTerm("+=", RefObj(arg[0]),
		int2Obj(1)), NULL);
}

static void f_pre_dec(EfiFunc *func, void *rval, void **arg)
{
	Val_obj(rval) = EvalObj(AssignTerm("-=", RefObj(arg[0]),
		int2Obj(1)), NULL);
}

static void f_post_inc(EfiFunc *func, void *rval, void **arg)
{
	EfiObj *x;
	EfiObj *copy;

	x = arg[0];
	copy = ConstObj(x->type, x->data);
	UnrefEval(AssignTerm("+=", RefObj(x), int2Obj(1)));
	Val_obj(rval) = copy;
}

static void f_post_dec(EfiFunc *func, void *rval, void **arg)
{
	EfiObj *x;
	EfiObj *copy;

	x = arg[0];
	copy = ConstObj(x->type, x->data);
	UnrefEval(AssignTerm("-=", RefObj(x), int2Obj(1)));
	Val_obj(rval) = copy;
}


/*	Zuweisungsoperator für Vektoren
*/

static void v_assign(EfiFunc *func, void *rval, void **arg)
{
	EfiVec *vec;
	EfiObjList *list;
	int i;

	vec = arg[0];
	list = Val_ptr(arg[1]);

	for (i = 0; i < vec->dim && list; i++, list = list->next)
		UnrefEval(AssignObj(Vector(vec, i),
			EvalObj(RefObj(list->obj), NULL)));

	if	(list != NULL)
		dbg_note(NULL, "[efmain:138]", NULL);

	CopyData(&Type_vec, rval, arg[0]);
}

static void v_xassign(EfiFunc *func, void *rval, void **arg)
{
	EfiVec *vec;
	EfiObjList *list;
	int i;

	vec = arg[0];
	list = Val_ptr(arg[1]);

	for (i = 0; i < vec->dim && list; i++, list = list->next)
		UnrefEval(AssignTerm(func->name, Vector(vec, i),
			EvalObj(RefObj(list->obj), NULL)));

	if	(list != NULL)
		dbg_note(NULL, "[efmain:138]", NULL);

	CopyData(&Type_vec, rval, arg[0]);
}

/*	Verknüpfung von Listen
*/

typedef EfiObj *(*list_eval)(const char *name, EfiObj *left, EfiObj *right);

static void list_expr(list_eval eval, const char *name, void *rval, void **arg)
{
	EfiObjList *left;
	EfiObjList *right;
	EfiObjList *result, **ptr;
	EfiObj *obj;

	left = Val_list(arg[0]);
	right = Val_list(arg[1]);
	result = NULL;
	ptr = &result;

#if	0
	while (left && right)
	{
		obj = EvalObj(eval(name, RefObj(left->obj),
			RefObj(right->obj)), NULL);

		if	(obj == NULL)	break;

		*ptr = NewObjList(obj);
		ptr = &(*ptr)->next;
		left = left->next;
		right = right->next;
	}
#else
/*	Resultatvektor mit konstanten Werten der rechten Liste aufbauen
*/
	while (left && right)
	{
		obj = EvalObj(RefObj(right->obj), NULL);

		if	(obj && obj->lval)
		{
			*ptr = NewObjList(ConstObj(obj->type, obj->data));
			UnrefObj(obj);
		}
		else	*ptr = NewObjList(obj);

		ptr = &(*ptr)->next;
		left = left->next;
		right = right->next;
	}

/*	Listenelemente verknüpfen
*/
	left = Val_list(arg[0]);
	right = result;

	while (right)
	{
		right->obj = EvalObj(eval(name, RefObj(left->obj),
			right->obj), NULL);
		left = left->next;
		right = right->next;
	}
#endif

	Val_list(rval) = result;
}

static void list_binary(EfiFunc *func, void *rval, void **arg)
{
	list_expr(BinaryTerm, func->name, rval, arg);
}

static int do_listcmp(int *val, void **arg)
{
	EfiObjList *left = Val_list(arg[0]);
	EfiObjList *right = Val_list(arg[1]);

	while (left && right)
	{
		*val = Obj2int(BinaryTerm("cmp", RefObj(left->obj),
			RefObj(right->obj)));

		if	(*val != 0)	return 1;

		left = left->next;
		right = right->next;
	}

	if	(right)	*val = -1;
	else if	(left)	*val = 1;
	else		*val = 0;

	return 1;
}

static void list_cmp(EfiFunc *func, void *rval, void **arg)
{
	do_listcmp(rval, arg);
}

CMPFUNC(list_lt,<,do_listcmp)
CMPFUNC(list_le,<=,do_listcmp)
CMPFUNC(list_eq,==,do_listcmp)
CMPFUNC(list_ne,!=,do_listcmp)
CMPFUNC(list_ge,>=,do_listcmp)
CMPFUNC(list_gt,>,do_listcmp)

static EfiObj *do_listassign(const char *name, EfiObj *left, EfiObj *right)
{
	return AssignObj(left, right);
}

static void list_assign(EfiFunc *func, void *rval, void **arg)
{
	list_expr(do_listassign, NULL, rval, arg);
}

static void list_xassign(EfiFunc *func, void *rval, void **arg)
{
	list_expr(AssignTerm, func->name, rval, arg);
}

static void f_catlist(EfiFunc *func, void *rval, void **arg)
{
	EfiObjList *list, *result, **ptr;

	result = NULL;
	ptr = &result;

	for (list = Val_list(arg[0]); list != NULL; list = list->next)
	{
		*ptr = NewObjList(RefObj(list->obj));
		ptr = &(*ptr)->next;
	}

	for (list = Val_list(arg[1]); list != NULL; list = list->next)
	{
		*ptr = NewObjList(RefObj(list->obj));
		ptr = &(*ptr)->next;
	}

	Val_list(rval) = result;
}

static void f_pushlist (EfiFunc *func, void *rval, void **arg)
{
	EfiObjList *list, *x, *y;

	list = Val_list(arg[0]);

	for (x = Val_list(arg[1]); x != NULL; x = x->next)
	{
		EfiObj *obj = EvalObj(RefObj(x->obj), NULL);

		if	(obj && obj->lval)
		{
			y = NewObjList(ConstObj(obj->type, obj->data));
			UnrefObj(obj);
		}
		else	y = NewObjList(obj);

		y->next = list;
		list = y;
	}

	Val_list(arg[0]) = list;
}

static void f_toplist (EfiFunc *func, void *rval, void **arg)
{
	EfiObjList *list, **ptr, *x, *y;

	list = Val_list(arg[0]);
	ptr = &list;

	while (*ptr != NULL)
		ptr = &(*ptr)->next;

	for (x = Val_list(arg[1]); x != NULL; x = x->next)
	{
		EfiObj *obj = EvalObj(RefObj(x->obj), NULL);

		if	(obj && obj->lval)
		{
			y = NewObjList(ConstObj(obj->type, obj->data));
			UnrefObj(obj);
		}
		else	y = NewObjList(obj);

		*ptr = y;
		ptr = &(*ptr)->next;
	}

	Val_list(arg[0]) = list;
}

static void f_poplist (EfiFunc *func, void *rval, void **arg)
{
	EfiObjList *list = Val_list(arg[0]);
	Val_obj(rval) = list ? ReduceObjList(arg[0]) : RefObj(arg[1]);
}

static void f_ldata (EfiFunc *func, void *rval, void **arg)
{
	EfiObjList *list = Val_list(arg[0]);
	Val_obj(rval) = RefObj(list ? list->obj : arg[1]);
}

static EfiFuncDef fdef_obj[] = {
	{ FUNC_RESTRICTED, &Type_expr, "str ()", f_str2expr },
	{ FUNC_RESTRICTED, &Type_vfunc, "str ()", f_str2virfunc },
	{ FUNC_VIRTUAL, &Type_obj, "eval (Expr_t)", f_xeval },
	{ FUNC_VIRTUAL, &Type_obj, "eval (Expr_t, VarTab)", f_xeval2 },
	{ FUNC_VIRTUAL, &Type_obj, "operator[] (EfiVec, int)", v_index },
	{ FUNC_VIRTUAL, &Type_list, "operator[] (EfiVec, List_t)", v_list },

	{ FUNC_VIRTUAL, &Type_int, "dim (List_t)", l_dim },
	{ FUNC_VIRTUAL, &Type_int, "dim (EfiVec)", v_dim },
	{ FUNC_VIRTUAL, &Type_int, "dim (.)", c_dim },

	{ 0, &Type_obj, "Object::operator++ & ()", f_pre_inc },
	{ 0, &Type_obj, "Object::operator-- & ()", f_pre_dec },
	{ FUNC_VIRTUAL, &Type_obj, "operator++ (. &)", f_post_inc },
	{ FUNC_VIRTUAL, &Type_obj, "operator-- (. &)", f_post_dec },

	{ FUNC_VIRTUAL, &Type_bool, "operator< (., . )", any_lt },
	{ FUNC_VIRTUAL, &Type_bool, "operator<= (., . )", any_le },
	{ FUNC_VIRTUAL, &Type_bool, "operator== (., . )", any_eq },
	{ FUNC_VIRTUAL, &Type_bool, "operator!= (., . )", any_ne },
	{ FUNC_VIRTUAL, &Type_bool, "operator> (., . )", any_gt },
	{ FUNC_VIRTUAL, &Type_bool, "operator>= (., . )", any_ge },

	{ 0, &Type_obj, "Object::operator:= & (.)", base_assign },
	{ 0, &Type_obj, "Object::operator*= & (.)", assign_op },
	{ 0, &Type_obj, "Object::operator/= & (.)", assign_op },
	{ 0, &Type_obj, "Object::operator%= & (.)", assign_op },
	{ 0, &Type_obj, "Object::operator+= & (.)", assign_op },
	{ 0, &Type_obj, "Object::operator-= & (.)", assign_op },
	{ 0, &Type_obj, "Object::operator<<= & (.)", assign_op },
	{ 0, &Type_obj, "Object::operator>>= & (.)", assign_op },
	{ 0, &Type_obj, "Object::operator&= & (.)", assign_op },
	{ 0, &Type_obj, "Object::operator^= & (.)", assign_op },
	{ 0, &Type_obj, "Object::operator|= & (.)", assign_op },

	{ 0, &Type_vec, "EfiVec::operator= (List_t)", v_assign },
	{ 0, &Type_vec, "EfiVec::operator:= (List_t)", v_assign },
	{ 0, &Type_vec, "EfiVec::operator*= (List_t)", v_xassign },
	{ 0, &Type_vec, "EfiVec::operator/= (List_t)", v_xassign },
	{ 0, &Type_vec, "EfiVec::operator%= (List_t)", v_xassign },
	{ 0, &Type_vec, "EfiVec::operator+= (List_t)", v_xassign },
	{ 0, &Type_vec, "EfiVec::operator-= (List_t)", v_xassign },
	{ 0, &Type_vec, "EfiVec::operator<<= (List_t)", v_xassign },
	{ 0, &Type_vec, "EfiVec::operator>>= (List_t)", v_xassign },
	{ 0, &Type_vec, "EfiVec::operator&= (List_t)", v_xassign },
	{ 0, &Type_vec, "EfiVec::operator^= (List_t)", v_xassign },
	{ 0, &Type_vec, "EfiVec::operator|= (List_t)", v_xassign },

	{ FUNC_VIRTUAL, &Type_list, "operator+ (List_t, List_t)", list_binary },
	{ FUNC_VIRTUAL, &Type_list, "operator- (List_t, List_t)", list_binary },
	{ FUNC_VIRTUAL, &Type_list, "operator* (List_t, List_t)", list_binary },
	{ FUNC_VIRTUAL, &Type_list, "operator/ (List_t, List_t)", list_binary },
	{ FUNC_VIRTUAL, &Type_list, "operator% (List_t, List_t)", list_binary },

	{ FUNC_VIRTUAL, &Type_int, "cmp (List_t, List_t)", list_cmp },
	{ FUNC_VIRTUAL, &Type_bool, "operator< (List_t, List_t)", list_lt },
	{ FUNC_VIRTUAL, &Type_bool, "operator<= (List_t, List_t)", list_le },
	{ FUNC_VIRTUAL, &Type_bool, "operator== (List_t, List_t)", list_eq },
	{ FUNC_VIRTUAL, &Type_bool, "operator!= (List_t, List_t)", list_ne },
	{ FUNC_VIRTUAL, &Type_bool, "operator> (List_t, List_t)", list_gt },
	{ FUNC_VIRTUAL, &Type_bool, "operator>= (List_t, List_t)", list_ge },

	{ 0, &Type_list, "List_t::operator= (List_t)", list_assign },
	{ 0, &Type_list, "List_t::operator+= (List_t)", list_xassign },
	{ 0, &Type_list, "List_t::operator-= (List_t)", list_xassign },
	{ 0, &Type_list, "List_t::operator*= (List_t)", list_xassign },
	{ 0, &Type_list, "catlist (List_t , ...)", f_catlist },
	{ 0, &Type_void, "List_t::push (...)", f_pushlist },
	{ 0, &Type_void, "List_t::top (...)", f_toplist },
	{ 0, &Type_obj, "List_t::pop (. def = NULL)", f_poplist },
	{ 0, &Type_obj, "List_t::data (. def = NULL)", f_ldata },
};


/*	Initialisierung
*/

void CmdSetup_obj(void)
{
	AddFuncDef(fdef_obj, tabsize(fdef_obj));
}
