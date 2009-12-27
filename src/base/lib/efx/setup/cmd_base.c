/*
Basistypen

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
#include <EFEU/cmdsetup.h>
#include <EFEU/refdata.h>
#include <EFEU/CmdPar.h>

/*	Lösch- und Kopierfunktionen
*/

void Clean_obj(const Type_t *st, void *data)
{
	UnrefObj(Val_obj(data));
	Val_obj(data) = NULL;
}

void Copy_obj(const Type_t *st, void *tg, const void *src)
{
	Val_obj(tg) = RefObj(Val_obj(src));
}

void Clean_ref(const Type_t *st, void *data)
{
	rd_deref(Val_ptr(data));
	Val_ptr(data) = NULL;
}

void Copy_ref(const Type_t *st, void *tg, const void *src)
{
	Val_ptr(tg) = rd_refer(Val_ptr(src));
}


void Clean_str(const Type_t *st, void *data)
{
	memfree(Val_str(data));
	Val_str(data) = NULL;
}

void Copy_str(const Type_t *st, void *tg, const void *src)
{
	Val_str(tg) = mstrcpy(Val_str(src));
}


static Obj_t *Eval_void(const Type_t *type, const void *data)
{
	return NULL;
}

static Obj_t *Eval_name(const Type_t *type, const void *data)
{
	Obj_t *obj;
	CmdParVar_t *var;
	
	if	((obj = GetVar(NULL, Val_str(data), NULL)) != NULL)
		return obj;

	if	((var = CmdPar_var(NULL, Val_str(data), 0)) != NULL)
		return ResourceObj(&Type_str, var->name);

	return NewPtrObj(&Type_undef, mstrcpy(Val_str(data)));
}


/*	Einfache Pointertypen
*/

Type_t Type_void = COMPLEX_TYPE("void", 0, 0, NULL,
	NULL, Eval_void, NULL, NULL, NULL);
Type_t Type_ptr = STD_TYPE("_Ptr_", void *, NULL, NULL, NULL);
Type_t Type_obj = STD_TYPE("Object", Obj_t *, &Type_ptr, Clean_obj, Copy_obj);
Type_t Type_expr = STD_TYPE("Expr_t", Obj_t *, &Type_ptr, Clean_obj, Copy_obj);
Type_t Type_str = COMPLEX_TYPE("str", sizeof(char *), 0, IOData_str,
	&Type_ptr, NULL, Clean_str, Copy_str, NULL);
Type_t Type_type = STD_TYPE("Type_t", Type_t *, &Type_ptr, NULL, NULL);
Type_t Type_lval = STD_TYPE("Lval_t", Type_t *, &Type_type, NULL, NULL);
Type_t Type_name = EVAL_TYPE("_Name_", char *, Eval_name, Clean_str, Copy_str);
Type_t Type_undef = STD_TYPE("_undef_", char *, NULL, Clean_str, Copy_str);

/*	Referenztypen
*/

Type_t Type_ref = STD_TYPE("_Ref_", void *, &Type_ptr, Clean_ref, Copy_ref);
Type_t Type_func = REF_TYPE("Func", Func_t *);
Type_t Type_vfunc = REF_TYPE("VirFunc", VirFunc_t *);
Type_t Type_vtab = REF_TYPE("VarTab", VarTab_t *);
Type_t Type_io = REF_TYPE ("IO", io_t *);


/*	Datentypen
*/

Type_t Type_enum = SIMPLE_TYPE("_Enum_", int, NULL);
Type_t Type_bool = SIMPLE_TYPE("bool", int, NULL);
Type_t Type_char = SIMPLE_TYPE("char", uchar_t, NULL);
Type_t Type_byte = SIMPLE_TYPE("byte", char, NULL);
Type_t Type_short = SIMPLE_TYPE("short", short, NULL);
Type_t Type_int = SIMPLE_TYPE("int", int, &Type_bool);
Type_t Type_uint = SIMPLE_TYPE("unsigned", unsigned, NULL);
Type_t Type_long = SIMPLE_TYPE("long", long, NULL);
Type_t Type_size = SIMPLE_TYPE("size_t", unsigned long, NULL);
Type_t Type_float = SIMPLE_TYPE("float", float, NULL);
Type_t Type_double = SIMPLE_TYPE("double", double, NULL);

uchar_t Buf_char = 0;
char Buf_byte = 0;
short Buf_short = 0;
int Buf_int = 0;
unsigned Buf_uint = 0;
long Buf_long = 0;
unsigned long Buf_size = 0;
float Buf_float = 0.;
double Buf_double = 0.;


/*	Funktionen
*/

static void f_any2void (Func_t *func, void *rval, void **arg)
{
	;
}

static void f_mkvirfunc (Func_t *func, void *rval, void **arg)
{
	Val_vfunc(rval) = VirFunc(rd_refer(Val_func(arg[0])));
}

static void f_ofunc2vfunc (Func_t *func, void *rval, void **arg)
{
	ObjFunc_t *x = arg[0];
	Val_vfunc(rval) = VirFunc(rd_refer(x->func));
}


static void f_ptr_not (Func_t *func, void *rval, void **arg)
{
	Val_bool(rval) = Val_ptr(arg[0]) == NULL;
}

static void f_ptr_eq (Func_t *func, void *rval, void **arg)
{
	Val_bool(rval) = Val_ptr(arg[0]) == Val_ptr(arg[1]);
}

static void f_ptr_ne (Func_t *func, void *rval, void **arg)
{
	Val_bool(rval) = Val_ptr(arg[0]) != Val_ptr(arg[1]);
}

static void f_any_free (Func_t *func, void *rval, void **arg)
{
	register Obj_t *obj = arg[0];

	if	(obj)	CleanData(obj->type, obj->data);
}

static void f_vtab(Func_t *func, void *rval, void **arg)
{
	Val_vtab(rval) = VarTab(Val_str(arg[0]), Val_int(arg[1]));
}

static FuncDef_t func_base[] = {
	{ 0, &Type_vtab, "VarTab (str name = NULL, int bs = 0)", f_vtab },
	{ 0, &Type_void, "void (.)", f_any2void },
	{ 0, &Type_vfunc, "Func ()", f_mkvirfunc },
	{ 0, &Type_vfunc, "ObjFunc ()", f_ofunc2vfunc },
	{ FUNC_VIRTUAL, &Type_bool, "operator!() (_Ptr_)", f_ptr_not },
	{ FUNC_VIRTUAL, &Type_bool, "operator== (_Ptr_, _Ptr_)", f_ptr_eq },
	{ FUNC_VIRTUAL, &Type_bool, "operator!= (_Ptr_, _Ptr_)", f_ptr_ne },
	{ FUNC_VIRTUAL, &Type_void, "free (. &)", f_any_free },
};


void CmdSetup_base(void)
{
	AddType(&Type_void);
	AddType(&Type_obj);
	AddType(&Type_ptr);
	AddType(&Type_ref);
	AddType(&Type_type);
	AddType(&Type_lval);
	AddType(&Type_func);
	AddType(&Type_vfunc);
	AddType(&Type_ofunc);
	AddType(&Type_expr);

	AddType(&Type_enum);
	AddType(&Type_bool);
	AddType(&Type_byte);
	AddType(&Type_short);
	AddType(&Type_int);
	AddType(&Type_long);
	AddType(&Type_uint);
	AddType(&Type_size);
	AddType(&Type_float);
	AddType(&Type_double);
	AddType(&Type_char);
	AddType(&Type_str);
	AddType(&Type_io);
	AddType(&Type_vtab);

	AddFuncDef(func_base, tabsize(func_base));
}
