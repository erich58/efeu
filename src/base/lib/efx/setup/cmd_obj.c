/*	Allgemeine Funktionen
	(c) 1994 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6

	Version 0.4
*/

#include <EFEU/object.h>
#include <EFEU/cmdconfig.h>

/*	Ausdrücke auswerten
*/

static void f_str2expr(Func_t *func, void *rval, void **arg)
{
	io_t *io;

	io = io_cstr(Val_str(arg[0]));
	Val_obj(rval) = Parse_cmd(io);
	io_close(io);
}

CEXPR(f_str2virfunc, Val_vfunc(rval) = VirFunc(GetGlobalFunc(Val_str(arg[0]))))
CEXPR(f_xeval, Val_obj(rval) = EvalExpression(Val_obj(arg[0])))

static void f_xeval2 (Func_t *func, void *rval, void **arg)
{
	PushVarTab(RefVarTab(Val_vtab(arg[1])), NULL);
	Val_obj(rval) = EvalExpression(Val_obj(arg[0]));
	PopVarTab();
}

/*	Dimensionen und Indizes
*/

CEXPR(l_dim, Val_int(rval) = ObjListLen(Val_list(arg[0])))
CEXPR(v_dim, Val_int(rval) = ((Vec_t *) arg[0])[0].dim)
CEXPR(c_dim, Val_int(rval) = 1)

CEXPR(v_index, Val_obj(rval) = Vector(arg[0], Val_int(arg[1])))

static void v_list(Func_t *func, void *rval, void **arg)
{
	ObjList_t *base, *list, **ptr;

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

static void base_assign(Func_t *func, void *rval, void **arg)
{
	Val_obj(rval) = AssignTerm("=", RefObj(arg[0]), RefObj(arg[1]));
}

static void assign_op(Func_t *func, void *rval, void **arg)
{
	Obj_t *left, *right;
	char *p;

	left = RefObj(arg[0]);
	p = mstrncpy(func->name, strlen(func->name) - 1);
	right = BinaryTerm(p, RefObj(left), RefObj(arg[1]));
	memfree(p);
	Val_obj(rval) = AssignObj(left, right);
}

/*	Increment- und Dekrementoperatoren
*/

static void f_pre_inc(Func_t *func, void *rval, void **arg)
{
	Val_obj(rval) = EvalObj(AssignTerm("+=", RefObj(arg[0]),
		int2Obj(1)), NULL);
}

static void f_pre_dec(Func_t *func, void *rval, void **arg)
{
	Val_obj(rval) = EvalObj(AssignTerm("-=", RefObj(arg[0]),
		int2Obj(1)), NULL);
}

static void f_post_inc(Func_t *func, void *rval, void **arg)
{
	Obj_t *x;
	Obj_t *copy;

	x = arg[0];
	copy = ConstObj(x->type, x->data);
	UnrefEval(AssignTerm("+=", RefObj(x), int2Obj(1)));
	Val_obj(rval) = copy;
}

static void f_post_dec(Func_t *func, void *rval, void **arg)
{
	Obj_t *x;
	Obj_t *copy;

	x = arg[0];
	copy = ConstObj(x->type, x->data);
	UnrefEval(AssignTerm("-=", RefObj(x), int2Obj(1)));
	Val_obj(rval) = copy;
}


/*	Zuweisungsoperator für Vektoren
*/

static void v_assign(Func_t *func, void *rval, void **arg)
{
	Vec_t *vec;
	ObjList_t *list;
	int i;

	vec = arg[0];
	list = Val_ptr(arg[1]);

	for (i = 0; i < vec->dim && list; i++, list = list->next)
		UnrefEval(AssignObj(Vector(vec, i),
			EvalObj(RefObj(list->obj), NULL)));

	if	(list != NULL)
		errmsg(MSG_EFMAIN, 138);

	CopyData(&Type_vec, rval, arg[0]);
}

static void v_xassign(Func_t *func, void *rval, void **arg)
{
	Vec_t *vec;
	ObjList_t *list;
	int i;

	vec = arg[0];
	list = Val_ptr(arg[1]);

	for (i = 0; i < vec->dim && list; i++, list = list->next)
		UnrefEval(AssignTerm(func->name, Vector(vec, i),
			EvalObj(RefObj(list->obj), NULL)));

	if	(list != NULL)
		errmsg(MSG_EFMAIN, 138);

	CopyData(&Type_vec, rval, arg[0]);
}

/*	Verknüpfung von Listen
*/

typedef Obj_t *(*list_eval)(const char *name, Obj_t *left, Obj_t *right);

static void list_expr(list_eval eval, const char *name, void *rval, void **arg)
{
	ObjList_t *left;
	ObjList_t *right;
	ObjList_t *result, **ptr;
	Obj_t *obj;

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

		if	(obj && obj->lref)
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

static void list_binary(Func_t *func, void *rval, void **arg)
{
	list_expr(BinaryTerm, func->name, rval, arg);
}

static Obj_t *do_listassign(const char *name, Obj_t *left, Obj_t *right)
{
	return AssignObj(left, right);
}

static void list_assign(Func_t *func, void *rval, void **arg)
{
	list_expr(do_listassign, NULL, rval, arg);
}

static void list_xassign(Func_t *func, void *rval, void **arg)
{
	list_expr(AssignTerm, func->name, rval, arg);
}

static void f_catlist(Func_t *func, void *rval, void **arg)
{
	ObjList_t *list, *result, **ptr;

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

static void f_pushlist (Func_t *func, void *rval, void **arg)
{
	ObjList_t *list, *x, *y;

	list = Val_list(arg[0]);

	for (x = Val_list(arg[1]); x != NULL; x = x->next)
	{
		Obj_t *obj = EvalObj(RefObj(x->obj), NULL);

		if	(obj && obj->lref)
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

static void f_toplist (Func_t *func, void *rval, void **arg)
{
	ObjList_t *list, **ptr, *x, *y;

	list = Val_list(arg[0]);
	ptr = &list;

	while (*ptr != NULL)
		ptr = &(*ptr)->next;

	for (x = Val_list(arg[1]); x != NULL; x = x->next)
	{
		Obj_t *obj = EvalObj(RefObj(x->obj), NULL);

		if	(obj && obj->lref)
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

static void f_poplist (Func_t *func, void *rval, void **arg)
{
	ObjList_t *list = Val_list(arg[0]);
	Val_obj(rval) = list ? ReduceObjList(arg[0]) : RefObj(arg[1]);
}

static void f_ldata (Func_t *func, void *rval, void **arg)
{
	ObjList_t *list = Val_list(arg[0]);
	Val_obj(rval) = RefObj(list ? list->obj : arg[1]);
}

static FuncDef_t fdef_obj[] = {
	{ FUNC_RESTRICTED, &Type_expr, "str ()", f_str2expr },
	{ FUNC_RESTRICTED, &Type_vfunc, "str ()", f_str2virfunc },
	{ FUNC_VIRTUAL, &Type_obj, "eval (Expr_t)", f_xeval },
	{ FUNC_VIRTUAL, &Type_obj, "eval (Expr_t, VarTab)", f_xeval2 },
	{ FUNC_VIRTUAL, &Type_obj, "operator[] (Vec_t, int)", v_index },
	{ FUNC_VIRTUAL, &Type_list, "operator[] (Vec_t, List_t)", v_list },

	{ FUNC_VIRTUAL, &Type_int, "dim (List_t)", l_dim },
	{ FUNC_VIRTUAL, &Type_int, "dim (Vec_t)", v_dim },
	{ FUNC_VIRTUAL, &Type_int, "dim (.)", c_dim },

	{ 0, &Type_obj, "Object::operator++ & ()", f_pre_inc },
	{ 0, &Type_obj, "Object::operator-- & ()", f_pre_dec },
	{ FUNC_VIRTUAL, &Type_obj, "operator++ (. &)", f_post_inc },
	{ FUNC_VIRTUAL, &Type_obj, "operator-- (. &)", f_post_dec },

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

	{ 0, &Type_vec, "Vec_t::operator= (List_t)", v_assign },
	{ 0, &Type_vec, "Vec_t::operator:= (List_t)", v_assign },
	{ 0, &Type_vec, "Vec_t::operator*= (List_t)", v_xassign },
	{ 0, &Type_vec, "Vec_t::operator/= (List_t)", v_xassign },
	{ 0, &Type_vec, "Vec_t::operator%= (List_t)", v_xassign },
	{ 0, &Type_vec, "Vec_t::operator+= (List_t)", v_xassign },
	{ 0, &Type_vec, "Vec_t::operator-= (List_t)", v_xassign },
	{ 0, &Type_vec, "Vec_t::operator<<= (List_t)", v_xassign },
	{ 0, &Type_vec, "Vec_t::operator>>= (List_t)", v_xassign },
	{ 0, &Type_vec, "Vec_t::operator&= (List_t)", v_xassign },
	{ 0, &Type_vec, "Vec_t::operator^= (List_t)", v_xassign },
	{ 0, &Type_vec, "Vec_t::operator|= (List_t)", v_xassign },

	{ FUNC_VIRTUAL, &Type_list, "operator+ (List_t, List_t)", list_binary },
	{ FUNC_VIRTUAL, &Type_list, "operator- (List_t, List_t)", list_binary },
	{ FUNC_VIRTUAL, &Type_list, "operator* (List_t, List_t)", list_binary },

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
