/*
Vektorfunktionen

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
#include <EFEU/CmpDef.h>
#include <EFEU/TestDef.h>

static EfiObj *vec_dim (const EfiObj *base, void *data)
{
	EfiVec *vec = base ? Val_ptr(base->data) : NULL;
	int dim = vec ? vec->buf.used : 0;
	return ConstObj(&Type_int, &dim);
}

static EfiObj *vec_type (const EfiObj *base, void *data)
{
	EfiVec *vec = base ? Val_ptr(base->data) : NULL;
	return vec ? ConstObj(&Type_type, &vec->type) : NULL;
}

static EfiMember var_vec[] = {
	{ "dim", &Type_int, vec_dim, NULL },
	{ "type", &Type_type, vec_type, NULL },
};


static void v_index (EfiFunc *func, void *rval, void **arg)
{ 
	Val_obj(rval) = Vector(Val_ptr(arg[0]), Val_int(arg[1]));
}

static void v_list (EfiFunc *func, void *rval, void **arg)
{
	EfiObjList *base, *list, **ptr;

	base = Val_list(arg[1]);
	list = NULL;
	ptr = &list;

	while (base != NULL)
	{
		*ptr = NewObjList(Vector(Val_ptr(arg[0]),
				Obj2int(RefObj(base->obj))));
		ptr = &(*ptr)->next;
		base = base->next;
	}

	Val_list(rval) = list;
}


static void v_dim (EfiFunc *func, void *rval, void **arg)
{ 
	EfiVec *vec = Val_ptr(arg[0]);
	Val_int(rval) = vec ? vec->buf.used : 0;
}

static void v_resize (EfiFunc *func, void *rval, void **arg)
{ 
	EfiVec_resize(Val_ptr(arg[0]), Val_int(arg[1]));
}

static void v_append (EfiFunc *func, void *rval, void **arg)
{ 
	EfiVec_append(Val_ptr(arg[0]), arg[1]);
}

static void v_delete (EfiFunc *func, void *rval, void **arg)
{ 
	EfiVec_delete(Val_ptr(arg[0]), Val_int(arg[1]), Val_int(arg[2]));
}

/*	sorting and seraching
*/

static CmpDef *qsort_par = NULL;

static int qsort_cmp (const void *a, const void *b)
{
	return cmp_data(qsort_par, a, b);
}

static void v_qsort (EfiFunc *func, void *rval, void **arg)
{
	EfiVec *vec = Val_ptr(arg[0]);
	CmpDef *save = qsort_par;
	
	if	(!vec || vec->buf.used < 2)	return;

	qsort_par = Val_ptr(arg[1]);
	qsort(vec->buf.data, vec->buf.used, vec->buf.elsize, qsort_cmp);
	qsort_par = save;
}

static void v_qsort_def (EfiFunc *func, void *rval, void **arg)
{
	EfiVec *vec = Val_ptr(arg[0]);
	CmpDef *save = qsort_par;
	
	if	(!vec || vec->buf.used < 2)	return;

	qsort_par = cmp_create(vec->type, Val_str(arg[1]), NULL);
	qsort(vec->buf.data, vec->buf.used, vec->buf.elsize, qsort_cmp);
	rd_deref(qsort_par);
	qsort_par = save;
}

static void v_uniq (EfiFunc *func, void *rval, void **arg)
{
	EfiVec *vec = Val_ptr(arg[0]);
	CmpDef *save = qsort_par;
	size_t n;
	
	if	(!vec || vec->buf.used < 2)	return;

	qsort_par = Val_ptr(arg[1]);

	if	(Val_bool(arg[2]))
		qsort(vec->buf.data, vec->buf.used, vec->buf.elsize, qsort_cmp);

	n = vuniq(vec->buf.data, vec->buf.used, vec->buf.elsize, qsort_cmp);
	EfiVec_delete(vec, n, vec->buf.used - n);
	qsort_par = save;
}

static void v_uniq_def (EfiFunc *func, void *rval, void **arg)
{
	EfiVec *vec = Val_ptr(arg[0]);
	CmpDef *save = qsort_par;
	size_t n;
	
	if	(!vec || vec->buf.used < 2)	return;

	qsort_par = cmp_create(vec->type, Val_str(arg[1]), NULL);

	if	(Val_bool(arg[2]))
		qsort(vec->buf.data, vec->buf.used, vec->buf.elsize, qsort_cmp);

	n = vuniq(vec->buf.data, vec->buf.used, vec->buf.elsize, qsort_cmp);
	EfiVec_delete(vec, n, vec->buf.used - n);
	rd_deref(qsort_par);
	qsort_par = save;
}

static EfiFunc *v_bsearch_func = NULL;

static int v_bsearch_cmp (const void *a, const void *b)
{
	int n;
	CallFunc(&Type_int, &n, v_bsearch_func, a, b);
	return n;
}

static void v_bsearch (EfiFunc *func, void *rval, void **arg)
{
	EfiVec *vec = Val_ptr(arg[0]);
	EfiObj *obj = arg[1];
	EfiFunc *save = v_bsearch_func;
	void *ptr = NULL;

	Val_obj(rval) = NULL;

	if	(vec == NULL || obj == NULL)
		return;

	v_bsearch_func = GetFunc(&Type_int, Val_vfunc(arg[2]), 2,
		obj->type, 0, vec->type, 0);

	if	(v_bsearch_func == NULL)
		dbg_note(NULL, "[efmain:nocmp]", NULL);
	else if	(vec->buf.used)
		ptr = bsearch(obj->data, vec->buf.data,
			vec->buf.used, vec->buf.elsize, v_bsearch_cmp);
	else	ptr = NULL;

	v_bsearch_func = save;

	if	(ptr == NULL)
	{
		Val_obj(rval) = ptr2Obj(NULL);
	}
	else if	(vec->type->list && vec->type->dim && !vec->type->list->next)
	{
		EfiStruct *var = vec->type->list;
		vec = NewEfiVec(var->type, ptr, var->dim);
		Val_obj(rval) = NewPtrObj(&Type_vec, vec);
	}
	else	Val_obj(rval) = LvalObj(&Lval_ref, vec->type, vec, ptr);
}

#if	0
static void v_test (EfiFunc *func, void *rval, void **arg)
{
	Val_int(rval) = EfiVec_test(Val_ptr(arg[0]), Val_ptr(arg[1]));
}

static void v_test_def (EfiFunc *func, void *rval, void **arg)
{
	EfiVec *vec = Val_ptr(arg[0]);

	if	(vec)
	{
		TestDef *def = TestDef_create(vec->type, Val_str(arg[1]));
		Val_int(rval) = EfiVec_test(vec, def);
		rd_deref(def);
	}
	else	Val_int(rval) = 0;
}

static void v_sel (EfiFunc *func, void *rval, void **arg)
{
	Val_int(rval) = EfiVec_select(Val_ptr(arg[0]), Val_ptr(arg[1]));
}

static void v_sel_def (EfiFunc *func, void *rval, void **arg)
{
	EfiVec *vec = Val_ptr(arg[0]);

	if	(vec)
	{
		TestDef *def = TestDef_create(vec->type, Val_str(arg[1]));
		Val_int(rval) = EfiVec_select(vec, def);
		rd_deref(def);
	}
	else	Val_int(rval) = 0;
}
#endif

/*	assignment
*/

static void v_assign (EfiFunc *func, void *rval, void **arg)
{
	EfiVec *vec;
	EfiObjList *list;
	size_t n;
	int i;

	vec = Val_ptr(arg[0]);
	list = Val_ptr(arg[1]);
	n = vec ? vec->buf.used : 0;

	for (i = 0; i < n && list; i++, list = list->next)
		UnrefEval(AssignObj(Vector(vec, i),
			EvalObj(RefObj(list->obj), NULL)));

	if	(list != NULL)
		dbg_note(NULL, "[efmain:138]", NULL);

	Val_ptr(rval) = rd_refer(vec);
}

static void v_xassign (EfiFunc *func, void *rval, void **arg)
{
	EfiVec *vec;
	EfiObjList *list;
	size_t n;
	int i;

	vec = Val_ptr(arg[0]);
	list = Val_ptr(arg[1]);
	n = vec ? vec->buf.used : 0;

	for (i = 0; i < n && list; i++, list = list->next)
		UnrefEval(AssignTerm(func->name, Vector(vec, i),
			EvalObj(RefObj(list->obj), NULL)));

	if	(list != NULL)
		dbg_note(NULL, "[efmain:138]", NULL);

	Val_ptr(rval) = rd_refer(vec);
}

/*	function table
*/

static EfiFuncDef fdef_vec[] = {
	{ FUNC_VIRTUAL, &Type_obj, "operator[] (EfiVec, int)", v_index },
	{ FUNC_VIRTUAL, &Type_obj, "operator[] (EfiVec, _Enum_)", v_index },
	{ FUNC_VIRTUAL, &Type_list, "operator[] (EfiVec, List_t)", v_list },
	{ FUNC_VIRTUAL, &Type_list, "operator[] (EfiVec)", Vec2List },

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

	{ FUNC_VIRTUAL, &Type_int, "dim (EfiVec)", v_dim },
	{ 0, &Type_void, "resize (EfiVec, int dim = 0)", v_resize },
	{ 0, &Type_void, "EfiVec::resize (int dim = 0)", v_resize },
	{ 0, &Type_void, "EfiVec::append (. obj)", v_append },
	{ 0, &Type_void, "EfiVec::delete (int pos, int dim = 1)", v_delete },

	{ FUNC_VIRTUAL, &Type_void, "qsort (EfiVec vec, str cmpdef)",
		v_qsort_def },
	{ FUNC_VIRTUAL, &Type_void, "qsort (EfiVec vec, CmpDef cmp)",
		v_qsort },
	{ 0, &Type_void, "EfiVec::qsort (CmpDef cmp)", v_qsort },

#if	0
	{ FUNC_VIRTUAL, &Type_bool, "test (EfiVec vec, str def)", v_test_def },
	{ FUNC_VIRTUAL, &Type_bool, "test (EfiVec vec, TestDef tst)", v_test },
	{ 0, &Type_bool, "EfiVec::test (TestDef tst)", v_test },

	{ FUNC_VIRTUAL, &Type_int, "select (EfiVec vec, str def)", v_sel_def },
	{ FUNC_VIRTUAL, &Type_int, "select (EfiVec vec, TestDef tst)", v_sel },
	{ 0, &Type_int, "EfiVec::select (TestDef tst)", v_test },
#endif

	{ FUNC_VIRTUAL, &Type_void,
		"uniq (EfiVec vec, str cmpdef, bool sort = true)",
		v_uniq_def },
	{ FUNC_VIRTUAL, &Type_void,
		"uniq (EfiVec vec, CmpDef cmp, bool sort = true)",
		v_uniq },
	{ 0, &Type_void, "EfiVec::uniq (CmpDef cmp, bool sort = true)",
		v_uniq },

	{ FUNC_VIRTUAL, &Type_obj, "bsearch (EfiVec vec, . key, VirFunc f)",
		v_bsearch },
	{ 0, &Type_obj, "EfiVec::bsearch (. key, VirFunc f)", v_bsearch },
};

/*	Initialisierung
*/

void CmdSetup_vec(void)
{
	AddType(&Type_vec);
	AddEfiMember(Type_vec.vtab, var_vec, tabsize(var_vec));
	AddFuncDef(fdef_vec, tabsize(fdef_vec));
}
