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
int PrintFloatPrec = 2;

static char *v_fmt_str = "%#*s";
static char *v_fmt_float = "%*.*f";
static char *v_fmt_int = "%*i";
static char *v_fmt_long = "%*lil";
static char *v_fmt_uint = "%*uu";
static char *v_fmt_size = "%*lulu";

static VarDef_t var_print[] = {
	{ "field_width", &Type_int, &PrintFieldWidth,
		":*:field with for formatting values\n"
		":de:Feldbreite für Werte\n" },
	{ "float_prec", &Type_int, &PrintFloatPrec,
		":*:floating point precission\n"
		":de:Genauigkeit von Gleitkommawerten\n" },
	{ "fmt_str", &Type_str, &v_fmt_str,
		":*:format key for strings\n"
		":de:Formatdefinition für Zeichenketten\n" },
	{ "fmt_float", &Type_str, &v_fmt_float,
		":*:format key for floating point numbers\n"
		":de:Formatdefinition für Gleitkommazahlen\n" },
	{ "fmt_int", &Type_str, &v_fmt_int,
		":*:format key for integer\n"
		":de:Formatdefinition für Ganzzahlwerte\n" },
	{ "fmt_long", &Type_str, &v_fmt_long,
		":*:format key for long integer\n"
		":de:Formatdefinition für lange Ganzzahlwerte\n" },
	{ "fmt_unsigned", &Type_str, &v_fmt_uint,
		":*:format key for unsigned integer\n"
		":de:Formatdefinition für vorzeichenfreie Ganzzahlwerte\n" },
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

static void fprint_ptr(Func_t *func, void *rval, void **arg)
{
	void *ptr = Val_ptr(arg[1]);

	if	(ptr)
	{
		RV = io_printf(Val_io(arg[0]), "%p", ptr);
	}
	else	RV = io_puts("NULL", Val_io(arg[0]));
}

static void fprint_type(Func_t *func, void *rval, void **arg)
{
	io_t *io = io_count(io_refer(Val_io(arg[0])));
	ListType(io, Val_type(arg[1]));
	RV = io_close(io);
}

static void fprint_bool(Func_t *func, void *rval, void **arg)
{
	RV = io_puts(Val_bool(arg[1]) ? "true" : "false", Val_io(arg[0]));
}

static void fprint_int(Func_t *func, void *rval, void **arg)
{
	RV = io_printf(Val_io(arg[0]), v_fmt_int,
		PrintFieldWidth, Val_int(arg[1]));
}

static void fprint_long(Func_t *func, void *rval, void **arg)
{
	RV = io_printf(Val_io(arg[0]), v_fmt_long,
		PrintFieldWidth, Val_long(arg[1]));
}

static void fprint_uint(Func_t *func, void *rval, void **arg)
{
	RV = io_printf(Val_io(arg[0]), v_fmt_uint,
		PrintFieldWidth, Val_uint(arg[1]));
}

static void fprint_size(Func_t *func, void *rval, void **arg)
{
	RV = io_printf(Val_io(arg[0]), v_fmt_size,
		PrintFieldWidth, (ulong_t) Val_size(arg[1]));
}

static void fprint_double(Func_t *func, void *rval, void **arg)
{
	RV = io_printf(Val_io(arg[0]), v_fmt_float,
		PrintFieldWidth, PrintFloatPrec, Val_double(arg[1]));
}

static void fprint_char(Func_t *func, void *rval, void **arg)
{
	RV = io_printf(Val_io(arg[0]), "%c", Val_char(arg[1]));
}

static void fprint_str(Func_t *func, void *rval, void **arg)
{
	RV = io_printf(Val_io(arg[0]), v_fmt_str,
		PrintFieldWidth, Val_str(arg[1]));
}

static void fprint_ref(Func_t *func, void *rval, void **arg)
{
	io_t *io = Val_io(arg[0]);
	refdata_t *rd = Val_ptr(arg[1]);

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


static void fprint_func(Func_t *func, void *rval, void **arg)
{
	RV = ListFunc(Val_io(arg[0]), Val_func(arg[1]));
}

static void fprint_vfunc(Func_t *func, void *rval, void **arg)
{
	io_t *io;
	VirFunc_t *x;
	int i, n;

	io = Val_io(arg[0]);
	x = Val_vfunc(arg[1]);

	if	(x != NULL)
	{
		Func_t **ftab = x->tab.data;

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

static void fprint_ofunc(Func_t *func, void *rval, void **arg)
{
	io_t *io;
	ObjFunc_t *ofunc;
	VirFunc_t *vfunc;
	int i, n;

	io = Val_io(arg[0]);
	ofunc = arg[1];
	n = 0;

	vfunc = VirFunc(ofunc->func);

	if	(vfunc != NULL)
	{
		Func_t **ftab = vfunc->tab.data;

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

static void fprint_name(Func_t *func, void *rval, void **arg)
{
	Val_int(rval) = io_puts(((Name_t *) arg[1])->name, Val_io(arg[0]));
}

static void fprint_mname(Func_t *func, void *rval, void **arg)
{
	Name_t *name;
	io_t *io;
	int n;

	io = Val_io(arg[0]);
	name = (Name_t *) arg[1];
	n = PrintObj(io, name->obj);
	n += io_puts(".", io);
	n += io_puts(name->name, io);
	Val_int(rval) = n;
}

static void fprint_sname(Func_t *func, void *rval, void **arg)
{
	Name_t *name;
	io_t *io;
	int n;

	io = Val_io(arg[0]);
	name = (Name_t *) arg[1];

	n = PrintObj(io, name->obj);
	n += io_puts("::", io);
	n += io_puts(name->name, io);
	Val_int(rval) = n;
}

static void fprint_undef(Func_t *func, void *rval, void **arg)
{
	Val_int(rval) = io_printf(Val_io(arg[0]), "(%s) %s",
		Type_undef.name, Val_str(arg[1]));
}


static void fprint_any(Func_t *func, void *rval, void **arg)
{
	io_t *io;
	Obj_t *obj;

	io = Val_io(arg[0]);
	obj = arg[1];
	RV = (obj && io) ? PrintAny(io, obj->type, obj->data) : 0;
}


static void fprint_vec(Func_t *func, void *rval, void **arg)
{
	char *delim;
	Vec_t *vec;
	io_t *io;
	int i, n;

	io = Val_io(arg[0]);
	vec = (Vec_t *) arg[1];

	n = io_puts(PrintListBegin, io);
	delim = NULL;

	for (i = 0; i < vec->dim; i++)
	{
		Obj_t *obj;

		obj = Vector(vec, i);
		n += io_puts(delim, io);
		n += PrintObj(io, obj);
		UnrefObj(obj);
		delim = PrintListDelim;
	}

	n += io_puts(PrintListEnd, io);
	Val_int(rval) = n;
}

static void fprint_vtab(Func_t *func, void *rval, void **arg)
{
	Val_int(rval) = ShowVarTab(Val_io(arg[0]), NULL, Val_ptr(arg[1]));
}


/*	Variable Argumentliste
*/

static int p_va_list(io_t *io, ObjList_t *list, char *sep)
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

static int p_list(io_t *io, ObjList_t *list)
{
	int n;

	n = io_puts(PrintListBegin, io);
	n += p_va_list(io, list, PrintListDelim);
	n += io_puts(PrintListEnd, io);
	return n;
}

static void fprint_list(Func_t *func, void *rval, void **arg)
{
	RV = p_list(Val_io(arg[0]), Val_list(arg[1]));
}

static void fprint_va_list(Func_t *func, void *rval, void **arg)
{
	RV = p_va_list(Val_io(arg[0]), Val_list(arg[1]), " ");
}

static void print_list(Func_t *func, void *rval, void **arg)
{
	RV = p_list(iostd, Val_list(arg[0]));
}

static void print_va_list(Func_t *func, void *rval, void **arg)
{
	RV = p_va_list(iostd, Val_list(arg[0]), " ");
}

static void sprint_list(Func_t *func, void *rval, void **arg)
{
	strbuf_t *sb = new_strbuf(0);
	io_t *io = io_strbuf(sb);
	p_list(io, Val_list(arg[0]));
	io_close(io);
	Val_str(rval) = sb2str(sb);
}

static void sprint_va_list(Func_t *func, void *rval, void **arg)
{
	strbuf_t *sb = new_strbuf(0);
	io_t *io = io_strbuf(sb);
	p_va_list(iostd, Val_list(arg[0]), " ");
	io_close(io);
	Val_str(rval) = sb2str(sb);
}

static void put_str(Func_t *func, void *rval, void **arg)
{
	io_puts(Val_str(arg[1]), Val_io(arg[0]));
	rd_refer(Val_io(arg[0]));
	Val_io(rval) = Val_io(arg[0]);
}

static void put_char(Func_t *func, void *rval, void **arg)
{
	io_putc(Val_char(arg[1]), Val_io(arg[0]));
	rd_refer(Val_io(arg[0]));
	Val_io(rval) = Val_io(arg[0]);
}

static void put_long(Func_t *func, void *rval, void **arg)
{
	io_printf(Val_io(arg[0]), "%*li", PrintFieldWidth, Val_long(arg[1]));
	rd_refer(Val_io(arg[0]));
	Val_io(rval) = Val_io(arg[0]);
}

static void put_obj(Func_t *func, void *rval, void **arg)
{
	PrintObj(Val_io(arg[0]), arg[1]);
	rd_refer(Val_io(arg[0]));
	Val_io(rval) = Val_io(arg[0]);
}


static FuncDef_t fdef_print[] = {
	{ FUNC_VIRTUAL, &Type_int, "fprint (IO, bool)", fprint_bool },
	{ FUNC_VIRTUAL, &Type_int, "fprint (IO, int)", fprint_int },
	{ FUNC_VIRTUAL, &Type_int, "fprint (IO, long)", fprint_long },
	{ FUNC_VIRTUAL, &Type_int, "fprint (IO, unsigned)", fprint_uint },
	{ FUNC_VIRTUAL, &Type_int, "fprint (IO, size_t)", fprint_size },
	{ FUNC_VIRTUAL, &Type_int, "fprint (IO, double)", fprint_double },
	{ FUNC_VIRTUAL, &Type_int, "fprint (IO, char)", fprint_char },
	{ FUNC_VIRTUAL, &Type_int, "fprint (IO, str)", fprint_str },

	{ FUNC_VIRTUAL, &Type_int, "fprint (IO, _Name_)", fprint_name },
	{ FUNC_VIRTUAL, &Type_int, "fprint (IO, _MemberName_)", fprint_mname },
	{ FUNC_VIRTUAL, &Type_int, "fprint (IO, _ScopeName_)", fprint_sname },
	{ FUNC_VIRTUAL, &Type_int, "fprint (IO, _undef_)", fprint_undef },
	{ FUNC_VIRTUAL, &Type_int, "fprint (IO, _Ptr_ *)", fprint_ptr },
	{ FUNC_VIRTUAL, &Type_int, "fprint (IO, _Ref_)", fprint_ref },
	{ FUNC_VIRTUAL, &Type_int, "fprint (IO, Type_t)", fprint_type },
	{ FUNC_VIRTUAL, &Type_int, "fprint (IO, Vec_t)", fprint_vec },
	{ FUNC_VIRTUAL, &Type_int, "fprint (IO, List_t)", fprint_list },
	{ FUNC_VIRTUAL, &Type_int, "fprint (IO, VarTab)", fprint_vtab },
	{ FUNC_VIRTUAL, &Type_int, "fprint (IO, Func)", fprint_func },
	{ FUNC_VIRTUAL, &Type_int, "fprint (IO, VirFunc)", fprint_vfunc },
	{ FUNC_VIRTUAL, &Type_int, "fprint (IO, ObjFunc)", fprint_ofunc },
	{ FUNC_VIRTUAL, &Type_int, "fprint (IO, .)", fprint_any },
	{ FUNC_VIRTUAL, &Type_int, "fprint (IO, ...)", fprint_va_list },

	{ FUNC_VIRTUAL, &Type_int, "print (List_t)", print_list },
	{ FUNC_VIRTUAL, &Type_int, "print (...)", print_va_list },
	{ FUNC_VIRTUAL, &Type_int, "sprint (List_t)", sprint_list },
	{ FUNC_VIRTUAL, &Type_int, "sprint (...)", sprint_va_list },

	{ FUNC_VIRTUAL, &Type_io, "operator<< (IO, char)", put_char },
	{ FUNC_VIRTUAL, &Type_io, "operator<< (IO, long)", put_long },
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
