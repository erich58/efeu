/*
Objekte mit Listen und Termen

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


static void clean_list (const Type_t *type, ObjList_t **tg);
static void copy_list (const Type_t *type, ObjList_t **tg, const ObjList_t **src);

static void clean_call (const Type_t *type, Expr_t *tg);
static void copy_call (const Type_t *type, Expr_t *tg, const Expr_t *src);
static Obj_t *eval_call (const Type_t *type, const Expr_t *expr);
static Obj_t *eval_list (const Type_t *type, const ObjList_t **list);

static void clean_ofunc (const Type_t *type, ObjFunc_t *tg);
static void copy_ofunc (const Type_t *type, ObjFunc_t *tg, const ObjFunc_t *src);

Type_t Type_list = STD_TYPE("List_t", ObjList_t *, &Type_ptr,
	(Clean_t) clean_list, (Copy_t) copy_list);
Type_t Type_ofunc = STD_TYPE("ObjFunc", ObjFunc_t, NULL,
	(Clean_t) clean_ofunc, (Copy_t) copy_ofunc);
Type_t Type_explist = EVAL_TYPE("_ExprList_", ObjList_t *,
	(Eval_t) eval_list, (Clean_t) clean_list, (Copy_t) copy_list);
Type_t Type_call = EVAL_TYPE("_Expr_", Expr_t,
	(Eval_t) eval_call, (Clean_t) clean_call, (Copy_t) copy_call);


static void clean_list(const Type_t *type, ObjList_t **list)
{
	DelObjList(*list);
	*list = NULL;
}


static void clean_call(const Type_t *type, Expr_t *expr)
{
	DelObjList(expr->list);
	expr->list = NULL;
	expr->par = NULL;
}


static void copy_list(const Type_t *type, ObjList_t **tg, const ObjList_t **src)
{
	*tg = RefObjList(*src);
}


static void copy_call(const Type_t *type, Expr_t *tg, const Expr_t *src)
{
	tg->eval = src->eval;
	tg->par = src->par;
	tg->list = RefObjList(src->list);
}


static Obj_t *eval_list (const Type_t *type, const ObjList_t **src)
{
	ObjList_t *list;

	list = EvalObjList(*src);
	return NewObj(&Type_list, &list);
}


static Obj_t *eval_call(const Type_t *st, const Expr_t *expr)
{
	return expr->eval ? expr->eval((void *) expr->par, expr->list) : NULL;
}


/*	Funktionsterme
*/


Obj_t *Obj_call(EvalExpr_t eval, void *par, ObjList_t *list)
{
	Expr_t expr;

	expr.eval = eval;
	expr.par = par;
	expr.list = list;
	return NewObj(&Type_call, &expr);
}


static void clean_ofunc(const Type_t *st, ObjFunc_t *tg)
{
	UnrefObj(tg->obj);
	rd_deref(tg->func);
	memset(tg, 0, st->size);
}


static void copy_ofunc(const Type_t *st, ObjFunc_t *tg, const ObjFunc_t *src)
{
	tg->obj = RefObj(src->obj);
	tg->func = rd_refer(src->func);
}
