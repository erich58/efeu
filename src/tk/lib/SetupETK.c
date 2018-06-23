/*
:*:	seting up esh-interface to TK
:de:	Esh-Interface zu TK initialisieren

$Copyright (C) 2002 Erich Frühstück
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

#include <GUI/ETK.h>
#include <EFEU/stdtype.h>
#include <EFEU/parsedef.h>
#include <EFEU/ArgList.h>
#include <EFEU/Info.h>
#include <ctype.h>

static ETK *etk = NULL;

static EfiObj *pf_tcl (IO *io, void *data)
{
	return ETK_parse(etk, io);
}

static EfiObj *pf_func (IO *io, void *data)
{
	ETK_func(etk, io);
	return NULL;
}

static EfiParseDef pdef[] = {
	{ "tcl", pf_tcl, NULL },
	{ "tclfunc", pf_func, NULL },
};

#if	HAS_TCL

#if	HAS_TCL_OBJ

#define	STR(x)	Tcl_NewStringObj(Val_str(x), -1)
#define	INT(x)	Tcl_NewIntObj(Val_int(x))
#define	DBL(x)	Tcl_NewDoubleObj(Val_double(x))
#define	BOOL(x)	Tcl_NewBooleanObj(Val_bool(x))

static void f_set (Tcl_Obj *obj)
{
	Tcl_SetObjResult(etk->interp, obj);
}

static void f_var_set (char *name, Tcl_Obj *obj)
{
#if	HAS_TCL_VAR2EX
	Tcl_SetVar2Ex(etk->interp, name, NULL, obj, TCL_GLOBAL_ONLY);
#else
	Tcl_Obj *idx = Tcl_NewStringObj((char *) name, -1);
	Tcl_IncrRefCount(idx);
	Tcl_ObjSetVar2(etk->interp, idx, NULL, obj, TCL_GLOBAL_ONLY);
	Tcl_DecrRefCount(idx);
#endif
}

#else	/* !HAS_TCL_OBJ */

static char val_buf[64];

#define	STR(x)	Val_str(x)
#define	INT(x)	(sprintf(val_buf, "%d", Val_int(x)), val_buf)
#define	DBL(x)	(sprintf(val_buf, "%.8g", Val_double(x)), val_buf)
#define	BOOL(x)	(sprintf(val_buf, "%d", Val_bool(x)), val_buf)

static void f_set (char *value)
{
	Tcl_SetResult(etk->interp, value, TCL_VOLATILE);
}

static void f_var_set (char *name, char *value)
{
	Tcl_SetVar(etk->interp, name, value, TCL_GLOBAL_ONLY);
}

#endif	/* HAS_TCL_OBJ */

static void f_set_str (EfiFunc *func, void *rval, void **arg)
{
	f_set(STR(arg[0]));
}

static void f_set_int (EfiFunc *func, void *rval, void **arg)
{
	f_set(INT(arg[0]));
}

static void f_set_dbl (EfiFunc *func, void *rval, void **arg)
{
	f_set(DBL(arg[0]));
}

static void f_set_bool (EfiFunc *func, void *rval, void **arg)
{
	f_set(BOOL(arg[0]));
}

static void f_var_str (EfiFunc *func, void *rval, void **arg)
{
	f_var_set(Val_str(arg[0]), STR(arg[1]));
}

static void f_var_int (EfiFunc *func, void *rval, void **arg)
{
	f_var_set(Val_str(arg[0]), INT(arg[1]));
}

static void f_var_dbl (EfiFunc *func, void *rval, void **arg)
{
	f_var_set(Val_str(arg[0]), DBL(arg[1]));
}

static void f_var_bool (EfiFunc *func, void *rval, void **arg)
{
	f_var_set(Val_str(arg[0]), BOOL(arg[1]));
}

static void f_get (EfiFunc *func, void *rval, void **arg)
{
	Val_obj(rval) = ETK_result(etk);
}

static void f_init (EfiFunc *func, void *rval, void **arg)
{
	if	(etk->tkstat)
	{
		Val_bool(rval) = 1;
	}
	else if	(Tk_Init(etk->interp) == TCL_OK)
	{
#if	TK_HAS_SAFE
		Tcl_StaticPackage(etk->interp, "Tk", Tk_Init, Tk_SafeInit);
#else
		Tcl_StaticPackage(etk->interp, "Tk", Tk_Init, NULL);
#endif

#if	TK_HAS_RCFILE
		Tcl_SetVar(etk->interp, "tcl_rcFileName",
			GetResource("ETKRC", "~/.etkrc"), TCL_GLOBAL_ONLY);
		Tcl_SourceRCFile(etk->interp);
#endif

		Val_bool(rval) = 1;
		etk->tkstat = 1;
	}
	else	Val_bool(rval) = 0;
}

static void f_main (EfiFunc *func, void *rval, void **arg)
{
	if	(etk->tkstat)
	{
		Tk_MainLoop();
		etk->tkstat = 0;
	}
}

static void f_event (EfiFunc *func, void *rval, void **arg)
{
	while (Tcl_DoOneEvent(TCL_ALL_EVENTS|TCL_DONT_WAIT))
		;
}

static void f_tcleval (EfiFunc *func, void *rval, void **arg)
{
	ArgList *argl;
	StrBuf *sb;
	IO *in, *out;
	char *p;
	int flag;
	int c;

	if	(func->vaarg)
	{
		EfiObjList *list;

		argl = arg_create();
		arg_cadd(argl, func->name);

		for (list = Val_list(arg[1]); list != NULL; list = list->next)
			arg_madd(argl, Obj2str(RefObj(list->obj)));
	}
	else if	(func->dim == 2)
	{
		argl = rd_refer(Val_ptr(arg[1]));
	}
	else	argl = NULL;

	sb = sb_acquire();
	in = io_cstr(Val_str(arg[0]));
	out = io_strbuf(sb);
	flag = 0;

	while ((c = io_getc(in)) != EOF)
	{
		if	(c == '$')
		{
			p = psubexpandarg(NULL, in, argl);

			if	(flag)
				io_xputs(p, out, "\"");
			else	io_puts(p, out);
		}
		else
		{
			if	(c == '"')
				flag = !flag;

			io_putc(c, out);
		}
	}

	io_close(out);
	ETK_eval(etk, sb_nul(sb));
	sb_release(sb);
	rd_deref(argl);
}

static void f_tclexpr (EfiFunc *func, void *rval, void **arg)
{
	f_tcleval(func, rval, arg);
	Val_obj(rval) = ETK_result(etk);
}

static EfiFuncDef fdef[] = {
	{ 0, &Type_obj, "tclget ()", f_get },
	{ FUNC_VIRTUAL, &Type_void, "tclset (bool val)", f_set_bool },
	{ FUNC_VIRTUAL, &Type_void, "tclset (int val)", f_set_int },
	{ FUNC_VIRTUAL, &Type_void, "tclset (double val)", f_set_dbl },
	{ FUNC_VIRTUAL, &Type_void, "tclset (str val = NULL)", f_set_str },
	{ FUNC_VIRTUAL, &Type_void, "tclvar (str name, bool val)",
		f_var_bool },
	{ FUNC_VIRTUAL, &Type_void, "tclvar (str name, int val)",
		f_var_int },
	{ FUNC_VIRTUAL, &Type_void, "tclvar (str name, double val)",
		f_var_dbl },
	{ FUNC_VIRTUAL, &Type_void, "tclvar (str name, str val = NULL)",
		f_var_str },
	{ FUNC_VIRTUAL, &Type_void, "tcleval (str cmd)",
		f_tcleval },
	{ FUNC_VIRTUAL, &Type_void, "tcleval (str cmd, ArgList list)",
		f_tcleval },
	{ FUNC_VIRTUAL, &Type_void, "tcleval (str cmd, ...)",
		f_tcleval },
	{ FUNC_VIRTUAL, &Type_obj, "tclexpr (str cmd)",
		f_tclexpr },
	{ FUNC_VIRTUAL, &Type_obj, "tclexpr (str cmd, ArgList list)",
		f_tclexpr },
	{ FUNC_VIRTUAL, &Type_obj, "tclexpr (str cmd, ...)",
		f_tclexpr },
	{ 0, &Type_bool, "tkinit ()", f_init },
	{ 0, &Type_void, "tkmain ()", f_main },
	{ 0, &Type_void, "tkevent ()", f_event },
};
#endif

/*
:*:
The function |$1| expands the esh-Interpreter to use tk-widgets.
This function is implicitly called on loading the shared library.
:de:
Die Funktion |$1| erweitert den esh-Interpreter zur Verwendung
von TK-Widgets.  Diese Funktion wird implizit beim
Laden der gemeinsam genutzen Programmbibliothek aufgerufen.
*/

void SetupETK (void)
{
	if	(etk)	return;

	SetupStd();
	etk = ETK_create();
	AddParseDef(pdef, tabsize(pdef));
#if	HAS_TCL
	AddFuncDef(fdef, tabsize(fdef));
	XInfoBrowser = ETKInfo;
#endif
}

/*
$SeeAlso
\mref{ETK(3)},
\mref{ETK_eval(3)},
\mref{ETK_func(3)},
\mref{ETK_parse(3)},
\mref{ETK_var(3)},
\mref{ETK(7)}.
*/
