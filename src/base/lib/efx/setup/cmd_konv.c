/*	Standardkonvertierungen
	(c) 1994 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6

	Version 0.4
*/

#include <EFEU/object.h>
#include <EFEU/cmdconfig.h>

void Ptr2bool (Func_t *func, void *rval, void **arg)
{
	Val_int(rval) = Val_ptr(arg[0]) != NULL;
}

void Vec2List (Func_t *func, void *rval, void **arg)
{
	long i, start, end;
	ObjList_t *list, **ptr;
	Vec_t *vec;

	list = NULL;
	ptr = &list;

	vec = arg[0];

	if	(func->dim > 1)
	{
		start = Val_int(arg[1]);
	}
	else	start = 0;

	if	(func->dim > 2)
	{
		end = start + Val_int(arg[2]);

		if	(end > vec->dim)	end = vec->dim;
	}
	else	end = vec->dim;

	for (i = start; i < end; i++)
	{
		*ptr = NewObjList(Vector(vec, i));
		ptr = &(*ptr)->next;
	}

	Val_list(rval) = list;
}

static void Type2vtab (Func_t *func, void *rval, void **arg)
{
	Type_t *type = Val_type(arg[0]);
	Val_vtab(rval) = type ? rd_refer(type->vtab) : NULL;
}

static void vtab2List (Func_t *func, void *rval, void **arg)
{
	ObjList_t *list, **ptr;
	VarTab_t *vtab;
	int i;
	
	vtab = Val_vtab(arg[0]);
	list = NULL;
	ptr = &list;

	for (i = 0; i < vtab->tab.dim; i++)
	{
		char *name = ((Var_t *) vtab->tab.tab[i])->name;

		if	(name == NULL)	continue;

		*ptr = NewObjList(str2Obj(mstrcpy(name)));
		ptr = &(*ptr)->next;
	}

	Val_list(rval) = list;
}

/*
static void any2obj (Func_t *func, void *rval, void **arg)
{
	Obj_t *x = arg[0];
	Val_obj(rval) = ConstObj(x->type, x->data);
}
*/

/*	Ganzzahlkonvertierungen
*/

CEXPR(k_bool2byte, Val_byte(rval) = Val_bool(arg[0]))
CEXPR(k_bool2short, Val_short(rval) = Val_bool(arg[0]))
CEXPR(k_byte2short, Val_short(rval) = Val_byte(arg[0]))
CEXPR(k_bool2int, Val_int(rval) = Val_bool(arg[0]))
CEXPR(k_byte2int, Val_int(rval) = Val_byte(arg[0]))
CEXPR(k_short2int, Val_int(rval) = Val_short(arg[0]))
CEXPR(k_bool2long, Val_long(rval) = Val_bool(arg[0]))
CEXPR(k_byte2long, Val_long(rval) = Val_byte(arg[0]))
CEXPR(k_short2long, Val_long(rval) = Val_short(arg[0]))
CEXPR(k_int2long, Val_long(rval) = Val_int(arg[0]))

CEXPR(k_byte2bool, Val_bool(rval) = Val_byte(arg[0]) != 0)
CEXPR(k_short2bool, Val_bool(rval) = Val_short(arg[0]) != 0)
CEXPR(k_int2bool, Val_bool(rval) = Val_int(arg[0]) != 0)
CEXPR(k_long2bool, Val_bool(rval) = Val_long(arg[0]) != 0)
CEXPR(k_short2byte, Val_byte(rval) = Val_short(arg[0]))
CEXPR(k_int2byte, Val_byte(rval) = Val_int(arg[0]))
CEXPR(k_long2byte, Val_byte(rval) = Val_long(arg[0]))
CEXPR(k_int2short, Val_short(rval) = Val_int(arg[0]))
CEXPR(k_long2short, Val_short(rval) = Val_long(arg[0]))
CEXPR(k_long2int, Val_int(rval) = Val_long(arg[0]))

/*	Gleitkommakonvertierung
*/

CEXPR(k_double2bool, Val_bool(rval) = Val_double(arg[0]) != 0.)
CEXPR(k_double2byte, Val_byte(rval) = (char) Val_double(arg[0]))
CEXPR(k_double2short, Val_short(rval) = (int) Val_double(arg[0]))
CEXPR(k_double2int, Val_int(rval) = (int) Val_double(arg[0]))
CEXPR(k_double2long, Val_long(rval) = (long) Val_double(arg[0]))

CEXPR(k_bool2double, Val_double(rval) = Val_bool(arg[0]))
CEXPR(k_byte2double, Val_double(rval) = Val_byte(arg[0]))
CEXPR(k_short2double, Val_double(rval) = Val_short(arg[0]))
CEXPR(k_int2double, Val_double(rval) = Val_int(arg[0]))
CEXPR(k_long2double, Val_double(rval) = Val_long(arg[0]))

CEXPR(k_double2float, Val_float(rval) = Val_double(arg[0]))
CEXPR(k_float2double, Val_double(rval) = Val_float(arg[0]))


static FuncDef_t konv_func[] = {
/*	Ganzzahlkonvertierungen
*/
	{ 0, &Type_byte, "bool ()", k_bool2byte },
	{ 0, &Type_short, "bool ()", k_bool2short },
	{ 0, &Type_short, "byte ()", k_byte2short },
	{ FUNC_PROMOTION, &Type_int, "bool ()", k_bool2int },
	{ FUNC_PROMOTION, &Type_int, "byte ()", k_byte2int },
	{ FUNC_PROMOTION, &Type_int, "short ()", k_short2int },
	{ 0, &Type_long, "bool ()", k_bool2long },
	{ 0, &Type_long, "byte ()", k_byte2long },
	{ 0, &Type_long, "short ()", k_short2long },
	{ FUNC_PROMOTION, &Type_long, "int ()", k_int2long },

	{ FUNC_RESTRICTED, &Type_bool, "byte ()", k_byte2bool },
	{ FUNC_RESTRICTED, &Type_bool, "short ()", k_short2bool },
	{ FUNC_RESTRICTED, &Type_bool, "int ()", k_int2bool },
	{ FUNC_RESTRICTED, &Type_bool, "long ()", k_long2bool },
	{ FUNC_RESTRICTED, &Type_byte, "short ()", k_short2byte },
	{ FUNC_RESTRICTED, &Type_byte, "int ()", k_int2byte },
	{ FUNC_RESTRICTED, &Type_byte, "long ()", k_long2byte },
	{ FUNC_RESTRICTED, &Type_short, "int ()", k_int2short },
	{ FUNC_RESTRICTED, &Type_short, "long ()", k_long2short },
	{ FUNC_RESTRICTED, &Type_int, "long ()", k_long2int },

/*	Gleitkommakonvertierungen
*/
	{ FUNC_PROMOTION, &Type_double, "float ()", k_float2double },
	{ 0, &Type_double, "bool ()", k_bool2double },
	{ 0, &Type_double, "byte ()", k_byte2double },
	{ 0, &Type_double, "short ()", k_short2double },
	{ 0, &Type_double, "int ()", k_int2double },
	{ 0, &Type_double, "long ()", k_long2double },

	{ FUNC_RESTRICTED, &Type_float, "double ()", k_double2float },
	{ 0, &Type_byte, "byte (double)", k_double2byte },
	{ 0, &Type_short, "short (double)", k_double2short },
	{ 0, &Type_long, "long (double)", k_double2long },
	{ 0, &Type_int, "int (double)", k_double2int },
	{ 0, &Type_bool, "bool (double)", k_double2bool },

/*	Sonstige Konvertierungen
*/
	{ 0, &Type_bool, "bool (_Ptr_)", Ptr2bool },
	{ 0, &Type_vtab, "Type_t ()", Type2vtab },
	/*
	{ FUNC_RESTRICTED, &Type_bool, "_Ptr_ ()", Ptr2bool },
	*/
	{ FUNC_RESTRICTED, &Type_list, "Vec_t ()", Vec2List },
	{ 0, &Type_list, "List_t (Vec_t, int start)", Vec2List },
	{ 0, &Type_list, "List_t (Vec_t, int start, int dim)", Vec2List },
	{ FUNC_RESTRICTED, &Type_list, "VarTab ()", vtab2List },
	/*
	{ FUNC_RESTRICTED, &Type_obj, "Object (.)", any2obj },
	*/
};

/*	Konvertierungen eintragen
*/

void CmdSetup_konv(void)
{
	AddFuncDef(konv_func, tabsize(konv_func));
}
