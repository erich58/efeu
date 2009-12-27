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


static void clean_list (const EfiType *type, void *tg);
static void copy_list (const EfiType *type, void *tg, const void *src);

static void clean_call (const EfiType *type, void *tg);
static void copy_call (const EfiType *type, void *tg, const void *src);
static EfiObj *eval_call (const EfiType *type, const void *ptr);
static EfiObj *eval_list (const EfiType *type, const void *ptr);

static void clean_ofunc (const EfiType *type, void *tg);
static void copy_ofunc(const EfiType *st, void *tg, const void *src);

EfiType Type_list = STD_TYPE("List_t", EfiObjList *, &Type_ptr,
	clean_list, copy_list);
EfiType Type_ofunc = STD_TYPE("ObjFunc", EfiObjFunc, NULL,
	clean_ofunc, copy_ofunc);
EfiType Type_explist = EVAL_TYPE("_ExprList_", EfiObjList *,
	eval_list, clean_list, copy_list);
EfiType Type_call = EVAL_TYPE("_Expr_", EfiExpr,
	eval_call, clean_call, copy_call);


static void clean_list (const EfiType *type, void *data)
{
	EfiObjList **list = data;
	DelObjList(*list);
	*list = NULL;
}


static void clean_call (const EfiType *type, void *data)
{
	EfiExpr *expr = data;
	DelObjList(expr->list);
	expr->list = NULL;
	expr->par = NULL;
}


static void copy_list(const EfiType *type, void *tg, const void *src)
{
	Val_ptr(tg) = RefObjList(Val_ptr(src));
}


static void copy_call(const EfiType *type, void *tptr, const void *sptr)
{
	EfiExpr *tg = tptr;
	const EfiExpr *src = sptr;
	tg->eval = src->eval;
	tg->par = src->par;
	tg->list = RefObjList(src->list);
}


static EfiObj *eval_list (const EfiType *type, const void *ptr)
{
	const EfiObjList *src = Val_ptr(ptr);
	EfiObjList *list = EvalObjList(src);
	return NewObj(&Type_list, &list);
}


static EfiObj *eval_call(const EfiType *st, const void *ptr)
{
	const EfiExpr *expr = ptr;
	return expr->eval ? expr->eval((void *) expr->par, expr->list) : NULL;
}


/*	Funktionsterme
*/


EfiObj *Obj_call(EfiObj *(*eval) (void *par, const EfiObjList *list), void *par, EfiObjList *list)
{
	EfiExpr expr;

	expr.eval = eval;
	expr.par = par;
	expr.list = list;
	return NewObj(&Type_call, &expr);
}


static void clean_ofunc(const EfiType *st, void *tg)
{
	EfiObjFunc *ofunc = tg;
	UnrefObj(ofunc->obj);
	rd_deref(ofunc->func);
	memset(ofunc, 0, st->size);
}


static void copy_ofunc(const EfiType *st, void *tptr, const void *sptr)
{
	EfiObjFunc *tg = tptr;
	const EfiObjFunc *src = sptr;
	tg->obj = RefObj(src->obj);
	tg->func = rd_refer(src->func);
}
