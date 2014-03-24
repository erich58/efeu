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
#include <EFEU/printobj.h>
#include <ctype.h>

/*	Lösch- und Kopierfunktionen
*/

void Clean_obj(const EfiType *st, void *data, int mode)
{
	UnrefObj(Val_obj(data));
	Val_obj(data) = NULL;
}

void Copy_obj(const EfiType *st, void *tg, const void *src)
{
	Val_obj(tg) = RefObj(Val_obj(src));
}

void Clean_ref(const EfiType *st, void *data, int mode)
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

int Print_str (const EfiType *st, const void *data, IO *io)
{
	char *str = Val_str(data);

	if	(str)
	{
		int n = io_puts("\"", io);
		n += io_xputs(str, io, "\"");
		n += io_puts("\"", io);
		return n;
	}
	else	return io_puts("NULL", io);
}

void Clean_str(const EfiType *st, void *data, int mode)
{
	memfree(Val_str(data));
	Val_str(data) = NULL;
}

static void debug_str (char * const *src, const char *tg)
{
	static int stat = -1;

	if	(stat < 0)
	{
		char *p = getenv("STRDEBUG");
		stat = p ? atoi(p) : 0;
	}

	if	(stat > 0)
		io_xprintf(ioerr, "Copy_str(%p, %p=%#s)\n", tg, src, *src);
}

void Copy_str(const EfiType *st, void *tg, const void *src)
{
	debug_str(src, tg);
	Val_str(tg) = mstrcpy(Val_str(src));
}

void Memory_copy(const EfiType *st, void *tg, const void *src)
{
	memcpy(tg, src, st->size);
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

static int print_ptr (const EfiType *st, const void *data, IO *io)
{
	if	(Val_ptr(data))
	{
		return io_printf(io, "_Ptr_(%p)", Val_ptr(data));
	}
	else	return io_puts("NULL", io);
}

static int print_type (const EfiType *st, const void *data, IO *io)
{
	const EfiType *type = * (const EfiType **) data;
	return io_puts(type ? type->name : ".", io);
}

static int print_obj (const EfiType *st, const void *data, IO *io)
{
	return PrintObj(io, Val_obj(data));
}

static int print_expr (const EfiType *st, const void *data, IO *io)
{
	int n = io_puts("[", io);
	n += PrintObj(io, Val_obj(data));
	return n + io_puts("]", io);
}

static int std_name (const char *name)
{
	if	(!isalpha(*name) && *name != '_')	return 0;

	for (name++; *name; name++)
		if	(!isalnum(*name) && *name != '_')	return 0;

	return 1;
}

static int print_name (const EfiType *st, const void *data, IO *io)
{
	const char *name = * (const char **) data;

	if	(!name && !*name)
		return io_puts("operator\"\"", io);

	if	(std_name(name))
		return io_puts(name, io);

	return io_xprintf(io, "operator%#s", name);
}

/*	Einfache Pointertypen
*/

EfiType Type_void = COMPLEX_TYPE("void", NULL, 0, 0, NULL, NULL, NULL, \
	NULL, Eval_void, NULL, NULL, 0);
EfiType Type_ptr = PTR_TYPE("_Ptr_", void *, NULL,
	print_ptr, NULL, NULL);
EfiType Type_obj = PTR_TYPE("Object", EfiObj *, &Type_ptr,
	print_obj, Clean_obj, Copy_obj);
EfiType Type_expr = PTR_TYPE("Expr_t", EfiObj *, &Type_ptr,
	print_expr, Clean_obj, Copy_obj);
EfiType Type_str = COMPLEX_TYPE("str", "char *", sizeof(char *), 0,
	Read_str, Write_str, Print_str,
	&Type_ptr, NULL, Clean_str, Copy_str, TYPE_MALLOC);
EfiType Type_type = PTR_TYPE("Type_t", EfiType *, &Type_ptr,
	print_type, NULL, NULL);
EfiType Type_lval = PTR_TYPE("Lval_t", EfiType *, &Type_type,
	NULL, NULL, NULL);
EfiType Type_name = EVAL_TYPE("_Name_", char *, Eval_name,
	print_name, Clean_str, Copy_str);
EfiType Type_undef = PTR_TYPE("_undef_", char *, NULL,
	NULL, Clean_str, Copy_str);

/*	Referenztypen
*/

EfiType Type_ref = PTR_TYPE("_Ref_", void *, &Type_ptr,
	NULL, Clean_ref, Copy_ref);
EfiType Type_efi = REF_TYPE ("Efi", Efi *);
EfiType Type_func = REF_TYPE("Func", EfiFunc *);
EfiType Type_vfunc = REF_TYPE("VirFunc", EfiVirFunc *);
EfiType Type_vtab = REF_TYPE("VarTab", EfiVarTab *);
EfiType Type_io = REF_TYPE ("IO", IO *);


/*	Datentypen
*/

static int print_bool (const EfiType *type, const void *data, IO *io)
{
	return io_puts(Val_bool(data) ? "true" : "false", io);
}

static int print_char (const EfiType *type, const void *data, IO *io)
{
	return io_xprintf(io, "%#c", Val_char(data));
}

static int print_wchar (const EfiType *type, const void *data, IO *io)
{
	return io_xprintf(io, "%#lc", *((int32_t *) data));
}

static int print_int (const EfiType *type, const void *data, IO *io)
{
	return io_xprintf(io, "%d", Val_int(data));
}

static int print_uint (const EfiType *type, const void *data, IO *io)
{
	return io_xprintf(io, "%uu", Val_uint(data));
}

static int print_float (const EfiType *type, const void *data, IO *io)
{
	return io_xprintf(io, "%.8g", Val_float(data));
}

static int print_double (const EfiType *type, const void *data, IO *io)
{
	return io_xprintf(io, "%.16g", Val_double(data));
}

EfiType Type_enum = COMPLEX_TYPE("_Enum_", "int", sizeof(int), 4, 
	Enum_read, Enum_write, NULL, NULL, NULL, NULL, Enum_copy, TYPE_ENUM);

EfiType Type_bool = SIMPLE_TYPE("bool", int, NULL, print_bool);
EfiType Type_char = SIMPLE_TYPE("char", unsigned char, NULL, print_char);
EfiType Type_wchar = SIMPLE_TYPE("wchar_t", int32_t, NULL, print_wchar);
EfiType Type_int = SIMPLE_TYPE("int", int, NULL, print_int);
EfiType Type_uint = SIMPLE_TYPE("unsigned", unsigned, NULL, print_uint);
EfiType Type_float = SIMPLE_TYPE("float", float, NULL, print_float);
EfiType Type_double = SIMPLE_TYPE("double", double, NULL, print_double);


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

	if	(obj)	CleanData(obj->type, obj->data, 0);
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
	AddType(&Type_efi);
	AddType(&Type_type);
	AddType(&Type_lval);
	AddType(&Type_func);
	AddType(&Type_vfunc);
	AddType(&Type_ofunc);
	AddType(&Type_expr);

	AddType(&Type_enum);
	AddType(&Type_bool);

	AddType(&Type_int8);
	AddType(&Type_uint8);
	AddType(&Type_int16);
	AddType(&Type_uint16);
	AddType(&Type_int);
	AddType(&Type_uint);

	AddType(&Type_int32);
	AddType(&Type_uint32);

	AddType(&Type_int64);
	AddType(&Type_varint);
	AddType(&Type_uint64);
	AddType(&Type_varsize);

	AddType(&Type_float);
	AddType(&Type_double);

	AddType(&Type_char);
	AddType(&Type_wchar);
	AddType(&Type_str);
	AddType(&Type_varstr);
	AddType(&Type_io);
	AddType(&Type_vtab);

	AddFuncDef(func_base, tabsize(func_base));
}
