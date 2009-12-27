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
#include <EFEU/EfiClass.h>

#define	CMPFUNC(name,op,cmp)	\
static void name (EfiFunc *func, void *rval, void **arg) \
{ int x; Val_int(rval) = cmp(&x, arg) && (x op 0) ? 1 : 0; }


/*	Ausdrücke auswerten
*/

static void f_str2expr (EfiFunc *func, void *rval, void **arg)
{
	IO *io;

	io = io_cstr(Val_str(arg[0]));
	Val_obj(rval) = Parse_block(io, EOF);
	io_close(io);
}

static void lval_class (EfiFunc *func, void *rval, void **arg)
{
	Val_obj(rval) = EfiClassExpr(RefObj(arg[0]), Val_str(arg[1]));
}

static void expr_class (EfiFunc *func, void *rval, void **arg)
{
	Val_obj(rval) = EfiClassExpr(RefObj(Val_obj(arg[0])), Val_str(arg[1]));
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
CEXPR(c_dim, Val_int(rval) = 1)

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

static void any_not (EfiFunc *func, void *rval, void **arg)
{
	Val_bool(rval) = ! Obj2bool(RefObj(arg[0]));
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
	UpdateLval(Val_obj(rval));
}

static void list_update (EfiFunc *func, void *rval, void **arg)
{
	UpdateObjList(Val_list(arg[0]));
}

static void obj_update (EfiFunc *func, void *rval, void **arg)
{
	EfiObj *obj = arg[0];

	if	(obj)
		UpdateLval(Val_obj(obj->data));
}

static void f_odata (EfiFunc *func, void *rval, void **arg)
{
	EfiObj *obj = arg[0];
	Val_obj(rval) = RefObj(obj ? Val_obj(obj->data) : arg[1]);
}

/*	sorting and seraching
*/

static EfiVirFunc *list_qsort_func = NULL;

static int list_qsort_cmp (const void *ap, const void *bp)
{
	EfiObj * const *a = ap;
	EfiObj * const *b = bp;
	EfiObjList list[2];

	list[0].obj = *a;
	list[0].next = list + 1;
	list[1].obj = *b;
	list[1].next = NULL;

	return Obj2int(EvalVirFunc(list_qsort_func, list));
}

static void list_qsort (EfiFunc *func, void *rval, void **arg)
{
	EfiObjList *list, *p;
	EfiObj **tab;
	EfiVirFunc *save;
	size_t n;
	
	list = Val_ptr(arg[0]);
	n = ObjListLen(list);

	if	(n < 1)	return;

	if	(!Val_vfunc(arg[1]))
	{
		log_note(NULL, "[efmain:nocmp]", NULL);
		return;
	}

	save = list_qsort_func;
	list_qsort_func = Val_vfunc(arg[1]);

	tab = lmalloc(n * sizeof *tab);

	for (n = 0, p = list; p != NULL; p = p->next)
		tab[n++] = p->obj;

	qsort(tab, n, sizeof tab[0], list_qsort_cmp);

	for (n = 0, p = list; p != NULL; p = p->next)
		p->obj = tab[n++];

	lfree(tab);
	list_qsort_func = save;
}

static void list_uniq (EfiFunc *func, void *rval, void **arg)
{
	EfiObjList *list, *p;
	EfiObj **tab;
	EfiVirFunc *save;
	size_t n, k;
	
	list = Val_ptr(arg[0]);

	for (n = 0, p = list; p != NULL; p = p->next)
		n++;

	if	(n < 2)	return;

	if	(!Val_vfunc(arg[1]))
	{
		log_note(NULL, "[efmain:nocmp]", NULL);
		return;
	}

	tab = lmalloc(n * sizeof *tab);

	for (n = 0, p = list; p != NULL; p = p->next)
		tab[n++] = p->obj;

	save = list_qsort_func;
	list_qsort_func = Val_vfunc(arg[1]);
	k = vuniq(tab, n, sizeof tab[0], list_qsort_cmp);
	list_qsort_func = save;
	
	for (n = 0, p = list; p != NULL; p = p->next)
		p->obj = tab[n++];

	lfree(tab);

	if	(k < n)
	{
		p = list;

		for (n = 1; n < k; n++)
			p = p->next;

		DelObjList(p->next);
		p->next = NULL;
	}
}

static void list_bsearch (EfiFunc *func, void *rval, void **arg)
{
	EfiObjList *list, *p;
	EfiObj *obj, **tab, **ptr;
	EfiVirFunc *save;
	size_t n;
	
	list = Val_ptr(arg[0]);
	obj = arg[1];
	n = ObjListLen(list);
	Val_obj(rval) = NULL;

	if	(n < 2)	return;

	if	(!Val_vfunc(arg[2]))
	{
		log_note(NULL, "[efmain:nocmp]", NULL);
		return;
	}

	save = list_qsort_func;
	list_qsort_func = Val_vfunc(arg[2]);

	tab = lmalloc(n * sizeof *tab);

	for (n = 0, p = list; p != NULL; p = p->next)
		tab[n++] = p->obj;

	if	((ptr = bsearch(&obj, tab, n, sizeof tab[0], list_qsort_cmp)))
		Val_obj(rval) = RefObj(*ptr);
	else	Val_obj(rval) = ptr2Obj(NULL);

	lfree(tab);
	list_qsort_func = save;
}

static void f_dump (EfiFunc *func, void *rval, void **arg)
{
	EfiObjList *list = Val_ptr(arg[0]);
	IO *out = Val_ptr(arg[1]);

	for (; list; list = list->next)
	{
		Obj_putkey(list->obj, out);
		io_puts("\n", out);
	}
}

static EfiFuncDef fdef_obj[] = {
	{ FUNC_RESTRICTED, &Type_expr, "str ()", f_str2expr },
	{ FUNC_VIRTUAL, &Type_expr, "classification (. &, str def)",
		lval_class },
	{ FUNC_VIRTUAL, &Type_expr, "classification (Expr_t, str def)",
		expr_class },
	{ FUNC_RESTRICTED, &Type_vfunc, "str ()", f_str2virfunc },
	{ FUNC_VIRTUAL, &Type_obj, "eval (Expr_t)", f_xeval },
	{ FUNC_VIRTUAL, &Type_obj, "eval (Expr_t, VarTab)", f_xeval2 },

	{ FUNC_VIRTUAL, &Type_int, "dim (List_t)", l_dim },
	{ FUNC_VIRTUAL, &Type_int, "dim (.)", c_dim },

	{ 0, &Type_obj, "Object::operator++ & ()", f_pre_inc },
	{ 0, &Type_obj, "Object::operator-- & ()", f_pre_dec },
	{ FUNC_VIRTUAL, &Type_obj, "operator++ (. &)", f_post_inc },
	{ FUNC_VIRTUAL, &Type_obj, "operator-- (. &)", f_post_dec },

	{ FUNC_VIRTUAL, &Type_bool, "operator!() (.)", any_not },

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

	{ FUNC_VIRTUAL|FUNC_PROMOTION, &Type_list,
		"operator+ (List_t, List_t)", list_binary },
	{ FUNC_VIRTUAL|FUNC_PROMOTION, &Type_list,
		"operator- (List_t, List_t)", list_binary },
	{ FUNC_VIRTUAL|FUNC_PROMOTION, &Type_list,
		"operator* (List_t, List_t)", list_binary },
	{ FUNC_VIRTUAL|FUNC_PROMOTION, &Type_list,
		"operator/ (List_t, List_t)", list_binary },
	{ FUNC_VIRTUAL|FUNC_PROMOTION, &Type_list,
		"operator% (List_t, List_t)", list_binary },

	{ FUNC_VIRTUAL|FUNC_PROMOTION, &Type_int,
		"cmp (List_t, List_t)", list_cmp },
	{ FUNC_VIRTUAL|FUNC_PROMOTION, &Type_bool,
		"operator< (List_t, List_t)", list_lt },
	{ FUNC_VIRTUAL|FUNC_PROMOTION, &Type_bool,
		"operator<= (List_t, List_t)", list_le },
	{ FUNC_VIRTUAL|FUNC_PROMOTION, &Type_bool,
		"operator== (List_t, List_t)", list_eq },
	{ FUNC_VIRTUAL|FUNC_PROMOTION, &Type_bool,
		"operator!= (List_t, List_t)", list_ne },
	{ FUNC_VIRTUAL|FUNC_PROMOTION, &Type_bool,
		"operator> (List_t, List_t)", list_gt },
	{ FUNC_VIRTUAL|FUNC_PROMOTION, &Type_bool,
		"operator>= (List_t, List_t)", list_ge },

	{ 0, &Type_list, "List_t::operator= (List_t)", list_assign },
	{ 0, &Type_list, "List_t::operator+= (List_t)", list_xassign },
	{ 0, &Type_list, "List_t::operator-= (List_t)", list_xassign },
	{ 0, &Type_list, "List_t::operator*= (List_t)", list_xassign },
	{ 0, &Type_list, "catlist (List_t , ...)", f_catlist },
	{ 0, &Type_void, "List_t::push (...)", f_pushlist },
	{ 0, &Type_void, "List_t::top (...)", f_toplist },
	{ 0, &Type_obj, "List_t::pop (. def = NULL)", f_poplist },
	{ 0, &Type_obj, "List_t::data (. def = NULL)", f_ldata },
	{ 0, &Type_void, "List_t::update ()", list_update },
	{ 0, &Type_void, "Object::update ()", obj_update },
	{ 0, &Type_obj, "Object::data (. def = NULL)", f_odata },
	{ FUNC_VIRTUAL, &Type_void, "qsort (List_t list, VirFunc f)",
		list_qsort },
	{ 0, &Type_void, "List_t::qsort (VirFunc f)", list_qsort },
	{ FUNC_VIRTUAL, &Type_void, "uniq (List_t list, VirFunc f)",
		list_uniq },
	{ 0, &Type_void, "List_t::uniq (VirFunc f)", list_uniq },
	{ FUNC_VIRTUAL, &Type_obj, "bsearch (List_t list, . key, VirFunc f)",
		list_bsearch },
	{ 0, &Type_obj, "List_t::bsearch (. key, VirFunc f)", list_bsearch },
	{ 0, &Type_void, "List_t::dump (IO *io = iostd)", f_dump },
};


/*	Initialisierung
*/

void CmdSetup_obj(void)
{
	AddFuncDef(fdef_obj, tabsize(fdef_obj));
}
