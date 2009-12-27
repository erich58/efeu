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

void Clean_obj(const EfiType *st, void *data)
{
	UnrefObj(Val_obj(data));
	Val_obj(data) = NULL;
}

void Copy_obj(const EfiType *st, void *tg, const void *src)
{
	Val_obj(tg) = RefObj(Val_obj(src));
}

void Clean_ref(const EfiType *st, void *data)
{
	rd_deref(Val_ptr(data));
	Val_ptr(data) = NULL;
}

void Copy_ref(const EfiType *st, void *tg, const void *src)
{
	Val_ptr(tg) = rd_refer(Val_ptr(src));
}


size_t Read_str (const EfiType *st, void *data, IO *io)
{
	int byte = io_getc(io);

	if	(byte == EOF)
	{
		Val_str(data) = NULL;
		return 0;
	}
	else if	(byte > 0)
	{
		size_t size = io_getval(io, byte);
		Val_str(data) = io_mread(io, size);
		return size + byte + 1;
	}
	else
	{
		Val_str(data) = NULL;
		return 1;
	}
}

size_t Write_str (const EfiType *st, const void *data, IO *io)
{
	return io_putstr(Val_str(data), io);
}

void Clean_str(const EfiType *st, void *data)
{
	memfree(Val_str(data));
	Val_str(data) = NULL;
}

void Copy_str(const EfiType *st, void *tg, const void *src)
{
	Val_str(tg) = mstrcpy(Val_str(src));
}


static EfiObj *Eval_void(const EfiType *type, const void *data)
{
	return NULL;
}

static EfiObj *Eval_name(const EfiType *type, const void *data)
{
	EfiObj *obj;
	CmdParVar *var;
	
	if	((obj = GetVar(NULL, Val_str(data), NULL)) != NULL)
		return obj;

	if	((var = CmdPar_var(NULL, Val_str(data), 0)) != NULL)
		return ResourceObj(&Type_str, var->name);

	return NewPtrObj(&Type_undef, mstrcpy(Val_str(data)));
}


/*	Einfache Pointertypen
*/

EfiType Type_void = COMPLEX_TYPE("void", 0, 0, NULL, NULL, \
	NULL, Eval_void, NULL, NULL);
EfiType Type_ptr = STD_TYPE("_Ptr_", void *, NULL, NULL, NULL);
EfiType Type_obj = STD_TYPE("Object", EfiObj *, &Type_ptr, Clean_obj, Copy_obj);
EfiType Type_expr = STD_TYPE("Expr_t", EfiObj *, &Type_ptr, Clean_obj, Copy_obj);
EfiType Type_str = COMPLEX_TYPE("str", sizeof(char *), 0, Read_str, Write_str,
	&Type_ptr, NULL, Clean_str, Copy_str);
EfiType Type_type = STD_TYPE("Type_t", EfiType *, &Type_ptr, NULL, NULL);
EfiType Type_lval = STD_TYPE("Lval_t", EfiType *, &Type_type, NULL, NULL);
EfiType Type_name = EVAL_TYPE("_Name_", char *, Eval_name, Clean_str, Copy_str);
EfiType Type_undef = STD_TYPE("_undef_", char *, NULL, Clean_str, Copy_str);

/*	Referenztypen
*/

EfiType Type_ref = STD_TYPE("_Ref_", void *, &Type_ptr, Clean_ref, Copy_ref);
EfiType Type_func = REF_TYPE("Func", EfiFunc *);
EfiType Type_vfunc = REF_TYPE("VirFunc", EfiVirFunc *);
EfiType Type_vtab = REF_TYPE("VarTab", EfiVarTab *);
EfiType Type_io = REF_TYPE ("IO", IO *);


/*	Datentypen
*/

EfiType Type_enum = SIMPLE_TYPE("_Enum_", int, NULL);
EfiType Type_bool = SIMPLE_TYPE("bool", int, NULL);
EfiType Type_char = SIMPLE_TYPE("char", unsigned char, NULL);
EfiType Type_byte = SIMPLE_TYPE("byte", char, NULL);
EfiType Type_short = SIMPLE_TYPE("short", short, NULL);
EfiType Type_int = SIMPLE_TYPE("int", int, &Type_bool);
EfiType Type_uint = SIMPLE_TYPE("unsigned", unsigned, NULL);
EfiType Type_long = SIMPLE_TYPE("long", long, NULL);
EfiType Type_size = SIMPLE_TYPE("size_t", unsigned long, NULL);
EfiType Type_float = SIMPLE_TYPE("float", float, NULL);
EfiType Type_double = SIMPLE_TYPE("double", double, NULL);

unsigned char Buf_char = 0;
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

static void f_any2void (EfiFunc *func, void *rval, void **arg)
{
	;
}

static void f_mkvirfunc (EfiFunc *func, void *rval, void **arg)
{
	Val_vfunc(rval) = VirFunc(rd_refer(Val_func(arg[0])));
}

static void f_ofunc2vfunc (EfiFunc *func, void *rval, void **arg)
{
	EfiObjFunc *x = arg[0];
	Val_vfunc(rval) = VirFunc(rd_refer(x->func));
}


static void f_ptr_not (EfiFunc *func, void *rval, void **arg)
{
	Val_bool(rval) = Val_ptr(arg[0]) == NULL;
}

static void f_ptr_eq (EfiFunc *func, void *rval, void **arg)
{
	Val_bool(rval) = Val_ptr(arg[0]) == Val_ptr(arg[1]);
}

static void f_ptr_ne (EfiFunc *func, void *rval, void **arg)
{
	Val_bool(rval) = Val_ptr(arg[0]) != Val_ptr(arg[1]);
}

static void f_any_free (EfiFunc *func, void *rval, void **arg)
{
	register EfiObj *obj = arg[0];

	if	(obj)	CleanData(obj->type, obj->data);
}

static void f_vtab(EfiFunc *func, void *rval, void **arg)
{
	Val_vtab(rval) = VarTab(Val_str(arg[0]), Val_int(arg[1]));
}

static EfiFuncDef func_base[] = {
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
