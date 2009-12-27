/*
Ausgabefunktionen

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
#include <EFEU/printobj.h>


#define	RV	Val_int(rval)


/*	Variablen
*/

int PrintFieldWidth = 0;
int PrintFloatPrec = 5;
int PrintTypeVerbosity = 3;

static char *v_fmt_str = "%#*s";
static char *v_fmt_float = "%#*.*g";
static char *v_fmt_int = "%*i";
static char *v_fmt_uint = "%*u";
static char *v_fmt_size = "%*lu";
static char *v_fmt_int64 = "%*lli";
static char *v_fmt_uint64 = "%*llu";

void SetFloatPrec (const char *str)
{
	char *p;
	char *flg;
	static char fmt_buf[8];

	if	(!str)	return;

	if	(*str == '#')
	{
		flg = "#";
		str++;
	}
	else	flg = "";

	PrintFloatPrec = strtol(str, &p, 0);

	switch (*p)
	{
	case 'e': case 'E':
	case 'f': case 'F':
	case 'g': case 'G':
		sprintf(fmt_buf, "%%%s*.*%c", flg, *p);
		break;
	default:
		sprintf(fmt_buf, "%%%s*.*g", flg);
		break;
	}

	memfree(v_fmt_float);
	v_fmt_float = fmt_buf;
}

int PrintDouble (IO *out, double val)
{
	return io_printf(out, v_fmt_float,
		PrintFieldWidth, PrintFloatPrec, val);
}

char *DoubleToString (double val)
{
	return msprintf(v_fmt_float,
		PrintFieldWidth, PrintFloatPrec, val);
}

static EfiVarDef var_print[] = {
	{ "field_width", &Type_int, &PrintFieldWidth,
		":*:field with for formatting values\n"
		":de:Feldbreite für Werte\n" },
	{ "float_prec", &Type_int, &PrintFloatPrec,
		":*:floating point precission\n"
		":de:Genauigkeit von Gleitkommawerten\n" },
	{ "PrintTypeVerbosity", &Type_int, &PrintTypeVerbosity,
		":*:verbosity of type representation\n"
		":de:Darstellungsausführlichkeit von Datentypen\n" },
	{ "fmt_str", &Type_str, &v_fmt_str,
		":*:format key for strings\n"
		":de:Formatdefinition für Zeichenketten\n" },
	{ "fmt_float", &Type_str, &v_fmt_float,
		":*:format key for floating point numbers\n"
		":de:Formatdefinition für Gleitkommazahlen\n" },
	{ "fmt_int", &Type_str, &v_fmt_int,
		":*:format key for integer\n"
		":de:Formatdefinition für Ganzzahlwerte\n" },
	{ "fmt_unsigned", &Type_str, &v_fmt_uint,
		":*:format key for unsigned integer\n"
		":de:Formatdefinition für vorzeichenfreie Ganzzahlwerte\n" },
	{ "fmt_int64", &Type_str, &v_fmt_int64,
		":*:format key for 64 bit integer\n"
		":de:Formatdefinition für 64-Bit Ganzzahlwerte\n" },
	{ "fmt_uint64", &Type_str, &v_fmt_uint64,
		":*:format key for unsigned 64 bit integer\n"
		":de:Formatdefinition für vorzeichenfreie 64-Bit Ganzzahlwerte\n" },
	{ "fmt_size", &Type_str, &v_fmt_size,
		":*:format key for size integer\n"
		":de:Formatdefinition für Größenzahlen\n" },
	{ "PrintListBegin", &Type_str, &PrintListBegin,
		":*:start string used in displaying lists\n"
		":de:Startzeichen für Listen\n" },
	{ "PrintListDelim", &Type_str, &PrintListDelim,
		":*:delimiter used in displaying lists\n"
		":de:Trennzeichen für Listen\n" },
	{ "PrintListEnd", &Type_str, &PrintListEnd,
		":*:end string used in displaying lists\n"
		":de:Abschlußzeichen für Listen\n" },
};


/*	Ausgabefunktionen
*/

static void fprint_ptr(EfiFunc *func, void *rval, void **arg)
{
	void *ptr = Val_ptr(arg[1]);

	if	(ptr)
	{
		RV = io_printf(Val_io(arg[0]), "%p", ptr);
	}
	else	RV = io_puts("NULL", Val_io(arg[0]));
}

static void fprint_type(EfiFunc *func, void *rval, void **arg)
{
	RV = PrintType(Val_io(arg[0]), Val_type(arg[1]), PrintTypeVerbosity);
}

static void fprint_bool(EfiFunc *func, void *rval, void **arg)
{
	RV = io_puts(Val_bool(arg[1]) ? "true" : "false", Val_io(arg[0]));
}

static void fprint_int(EfiFunc *func, void *rval, void **arg)
{
	RV = io_printf(Val_io(arg[0]), v_fmt_int,
		PrintFieldWidth, Val_int(arg[1]));
}

static void fprint_uint(EfiFunc *func, void *rval, void **arg)
{
	RV = io_printf(Val_io(arg[0]), v_fmt_uint,
		PrintFieldWidth, Val_uint(arg[1]));
}

static void fprint_int64(EfiFunc *func, void *rval, void **arg)
{
	RV = io_printf(Val_io(arg[0]), v_fmt_int64,
		PrintFieldWidth, *((int64_t *) arg[1]));
}

static void fprint_uint64(EfiFunc *func, void *rval, void **arg)
{
	RV = io_printf(Val_io(arg[0]), v_fmt_uint64,
		PrintFieldWidth, *((uint64_t *) arg[1]));
}

static void fprint_double(EfiFunc *func, void *rval, void **arg)
{
	RV = PrintDouble(Val_io(arg[0]), Val_double(arg[1]));
}

static void fprint_char(EfiFunc *func, void *rval, void **arg)
{
	RV = io_printf(Val_io(arg[0]), "%c", Val_char(arg[1]));
}

static void fprint_wchar(EfiFunc *func, void *rval, void **arg)
{
	RV = io_printf(Val_io(arg[0]), "%lc", Val_wchar(arg[1]));
}

static void fprint_str(EfiFunc *func, void *rval, void **arg)
{
	RV = io_printf(Val_io(arg[0]), v_fmt_str,
		PrintFieldWidth, Val_str(arg[1]));
}

static void fprint_ref(EfiFunc *func, void *rval, void **arg)
{
	IO *io = Val_io(arg[0]);
	RefData *rd = Val_ptr(arg[1]);

	if	(rd == NULL)
	{
		RV = io_puts("NULL", io);
	}
	else if	(rd->reftype == NULL)
	{
		RV = io_printf(io, "%p", rd);
	}
	else
	{
		char *p = rd_ident(rd);
		RV = io_printf(io, "%s: %s", rd->reftype->label, p);
		memfree(p);
	}
}


static void fprint_func(EfiFunc *func, void *rval, void **arg)
{
	RV = ListFunc(Val_io(arg[0]), Val_func(arg[1]));
}

static void fprint_vfunc(EfiFunc *func, void *rval, void **arg)
{
	IO *io;
	EfiVirFunc *x;
	int i, n;

	io = Val_io(arg[0]);
	x = Val_vfunc(arg[1]);

	if	(x != NULL)
	{
		EfiFunc **ftab = x->tab.data;

		for (i = n = 0; i < x->tab.used; i++)
		{
			n += ListFunc(io, ftab[i]);
			n += io_puts("\n", io);
			n++;
		}
	}
	else	n = io_puts("NULL", io);

	RV = n;
}

static void fprint_ofunc(EfiFunc *func, void *rval, void **arg)
{
	IO *io;
	EfiObjFunc *ofunc;
	EfiVirFunc *vfunc;
	int i, n;

	io = Val_io(arg[0]);
	ofunc = arg[1];
	n = 0;

	vfunc = VirFunc(ofunc->func);

	if	(vfunc != NULL)
	{
		EfiFunc **ftab = vfunc->tab.data;

		for (i = 0; i < vfunc->tab.used; i++)
		{
			n += io_puts("(", io);
			n += PrintObj(io, ofunc->obj);
			n += io_puts(") ", io);
			n += ListFunc(io, ftab[i]);
			n += io_puts("\n", io);
		}
	}
	else
	{
		n += io_puts("(", io);
		n += PrintObj(io, ofunc->obj);
		n += io_puts(") ", io);
		n = io_puts("NULL", io);
	}

	RV = n;
}

static void fprint_name(EfiFunc *func, void *rval, void **arg)
{
	Val_int(rval) = io_puts(((EfiName *) arg[1])->name, Val_io(arg[0]));
}

static void fprint_mname(EfiFunc *func, void *rval, void **arg)
{
	EfiName *name;
	IO *io;
	int n;

	io = Val_io(arg[0]);
	name = (EfiName *) arg[1];
	n = PrintObj(io, name->obj);
	n += io_puts(".", io);
	n += io_puts(name->name, io);
	Val_int(rval) = n;
}

static void fprint_sname(EfiFunc *func, void *rval, void **arg)
{
	EfiName *name;
	IO *io;
	int n;

	io = Val_io(arg[0]);
	name = (EfiName *) arg[1];

	n = PrintObj(io, name->obj);
	n += io_puts("::", io);
	n += io_puts(name->name, io);
	Val_int(rval) = n;
}

static void fprint_undef(EfiFunc *func, void *rval, void **arg)
{
	Val_int(rval) = io_printf(Val_io(arg[0]), "(%s) %s",
		Type_undef.name, Val_str(arg[1]));
}


static void fprint_any(EfiFunc *func, void *rval, void **arg)
{
	IO *io;
	EfiObj *obj;

	io = Val_io(arg[0]);
	obj = arg[1];
	RV = (obj && io) ? ShowAny(io, obj->type, obj->data) : 0;
}

static void fprint_obj (EfiFunc *func, void *rval, void **arg)
{
	IO *io;
	EfiObj *obj;

	io = Val_io(arg[0]);
	obj = Val_obj(arg[1]);

	if	(obj)
	{
		RV = io_puts("(Object) ", io);
		RV += PrintObj(io, obj);
	}
	else	RV = 0;
}

static void fprint_vec(EfiFunc *func, void *rval, void **arg)
{
	char *delim;
	EfiVec *vec;
	IO *io;
	int i, n, dim;

	io = Val_io(arg[0]);
	vec = Val_ptr(arg[1]);
	dim = vec ? vec->buf.used : 0;

	n = io_puts(PrintListBegin, io);
	delim = NULL;

	for (i = 0; i < dim; i++)
	{
		EfiObj *obj = Vector(vec, i);
		n += io_puts(delim, io);
		n += PrintObj(io, obj);
		UnrefObj(obj);
		delim = PrintListDelim;
	}

	n += io_puts(PrintListEnd, io);
	Val_int(rval) = n;
}

static void fprint_vtab(EfiFunc *func, void *rval, void **arg)
{
	Val_int(rval) = ShowVarTab(Val_io(arg[0]), NULL, Val_ptr(arg[1]));
}


/*	Variable Argumentliste
*/

static int p_va_list(IO *io, EfiObjList *list, char *sep)
{
	int n;
	char *delim;

	n = 0;
	delim = NULL;

	while (list != NULL)
	{
		n += io_puts(delim, io);
		n += PrintObj(io, list->obj);
		delim = sep;
		list = list->next;
	}

	return n;
}

static int p_list(IO *io, EfiObjList *list)
{
	int n;

	n = io_puts(PrintListBegin, io);
	n += p_va_list(io, list, PrintListDelim);
	n += io_puts(PrintListEnd, io);
	return n;
}

static void fprint_list(EfiFunc *func, void *rval, void **arg)
{
	RV = p_list(Val_io(arg[0]), Val_list(arg[1]));
}

static void fprint_va_list(EfiFunc *func, void *rval, void **arg)
{
	RV = p_va_list(Val_io(arg[0]), Val_list(arg[1]), " ");
}

static void put_str(EfiFunc *func, void *rval, void **arg)
{
	io_puts(Val_str(arg[1]), Val_io(arg[0]));
	rd_refer(Val_io(arg[0]));
	Val_io(rval) = Val_io(arg[0]);
}

static void put_char(EfiFunc *func, void *rval, void **arg)
{
	io_putc(Val_char(arg[1]), Val_io(arg[0]));
	rd_refer(Val_io(arg[0]));
	Val_io(rval) = Val_io(arg[0]);
}

static void put_obj(EfiFunc *func, void *rval, void **arg)
{
	ShowObj(Val_io(arg[0]), arg[1]);
	rd_refer(Val_io(arg[0]));
	Val_io(rval) = Val_io(arg[0]);
}


static EfiFuncDef fdef_print[] = {
	{ FUNC_VIRTUAL, &Type_int, "fprint (IO, bool)", fprint_bool },
	{ FUNC_VIRTUAL, &Type_int, "fprint (IO, int)", fprint_int },
	{ FUNC_VIRTUAL, &Type_int, "fprint (IO, unsigned)", fprint_uint },
	{ FUNC_VIRTUAL, &Type_int, "fprint (IO, varint)", fprint_int64 },
	{ FUNC_VIRTUAL, &Type_int, "fprint (IO, varsize)", fprint_uint64 },
	{ FUNC_VIRTUAL, &Type_int, "fprint (IO, double)", fprint_double },
	{ FUNC_VIRTUAL, &Type_int, "fprint (IO, char)", fprint_char },
	{ FUNC_VIRTUAL, &Type_int, "fprint (IO, wchar_t)", fprint_wchar },
	{ FUNC_VIRTUAL, &Type_int, "fprint (IO, str)", fprint_str },

	{ FUNC_VIRTUAL, &Type_int, "fprint (IO, _Name_)", fprint_name },
	{ FUNC_VIRTUAL, &Type_int, "fprint (IO, _MemberName_)", fprint_mname },
	{ FUNC_VIRTUAL, &Type_int, "fprint (IO, _ScopeName_)", fprint_sname },
	{ FUNC_VIRTUAL, &Type_int, "fprint (IO, _undef_)", fprint_undef },
	{ FUNC_VIRTUAL, &Type_int, "fprint (IO, restricted _Ptr_)", fprint_ptr },
	{ FUNC_VIRTUAL, &Type_int, "fprint (IO, _Ref_)", fprint_ref },
	{ FUNC_VIRTUAL, &Type_int, "fprint (IO, Type_t)", fprint_type },
	{ FUNC_VIRTUAL, &Type_int, "fprint (IO, Object)", fprint_obj },
	{ FUNC_VIRTUAL, &Type_int, "fprint (IO, EfiVec)", fprint_vec },
	{ FUNC_VIRTUAL, &Type_int, "fprint (IO, List_t)", fprint_list },
	{ FUNC_VIRTUAL, &Type_int, "fprint (IO, VarTab)", fprint_vtab },
	{ FUNC_VIRTUAL, &Type_int, "fprint (IO, Func)", fprint_func },
	{ FUNC_VIRTUAL, &Type_int, "fprint (IO, VirFunc)", fprint_vfunc },
	{ FUNC_VIRTUAL, &Type_int, "fprint (IO, ObjFunc)", fprint_ofunc },
	{ FUNC_VIRTUAL, &Type_int, "fprint (IO, .)", fprint_any },
	{ FUNC_VIRTUAL, &Type_int, "fprint (IO, ...)", fprint_va_list },

	{ FUNC_VIRTUAL, &Type_io, "operator<< (IO, char)", put_char },
	{ FUNC_VIRTUAL, &Type_io, "operator<< (IO, str)", put_str },
	{ FUNC_VIRTUAL, &Type_io, "operator<< (IO, .)", put_obj },
};


/*	Initialisierung
*/

void CmdSetup_print(void)
{
	AddVarDef(NULL, var_print, tabsize(var_print));
	AddFuncDef(fdef_print, tabsize(fdef_print));
}
