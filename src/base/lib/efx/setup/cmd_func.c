/*
Standardfunktionen

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

#include <EFEU/ftools.h>
#include <EFEU/object.h>
#include <EFEU/cmdconfig.h>
#include <EFEU/cmdeval.h>
#include <EFEU/printobj.h>
#include <EFEU/locale.h>
#include <EFEU/preproc.h>
#include <EFEU/Debug.h>
#include <EFEU/LangDef.h>
#include <ctype.h>

#define	INC_PFX	"include"

static void f_isdigit (EfiFunc *func, void *rval, void **arg)
{
	Val_int(rval) = isdigit(Val_int(arg[0])) ? 1 : 0;
}

static void f_isalpha (EfiFunc *func, void *rval, void **arg)
{
	Val_int(rval) = isalpha(Val_int(arg[0])) ? 1 : 0;
}

static void f_isalnum (EfiFunc *func, void *rval, void **arg)
{
	Val_int(rval) = isalnum(Val_int(arg[0])) ? 1 : 0;
}

static void f_isspace (EfiFunc *func, void *rval, void **arg)
{
	Val_int(rval) = isspace(Val_int(arg[0])) ? 1 : 0;
}


/*	Test auf Variablendefinition
*/

static void c_sizeof (EfiFunc *func, void *rval, void **arg)
{
	Val_int(rval) = ((EfiObj *) arg[0])->type->size;
}

static void u_sizeof (EfiFunc *func, void *rval, void **arg)
{
	Val_int(rval) = 0;
}

static void t_sizeof (EfiFunc *func, void *rval, void **arg)
{
	Val_int(rval) = Val_type(arg[0])->size;
}


static void f_declare (EfiFunc *func, void *rval, void **arg)
{
	StrBuf *sb;
	IO *tmp;

	sb = sb_acquire();
	tmp = io_strbuf(sb);
	PrintType(tmp, Val_type(arg[0]), Val_bool(arg[1]) ? 1 : 2);
	io_close(tmp);
	Val_str(rval) = sb_cpyrelease(sb);
}

static void f_whatis (EfiFunc *func, void *rval, void **arg)
{
	Val_str(rval) = Obj_ident(arg[0]);
}

static void f_align (EfiFunc *func, void *rval, void **arg)
{
	int sig = 1;
	int n = Val_int(arg[0]);
	int k = Val_int(arg[1]);

	if	(n == 0 || k == 0)
	{
		Val_int(rval) = n;
		return;
	}

	if	(k < 0)	sig = -sig, k = -k;
	if	(n < 0)	sig = -sig, n = -n;

	Val_int(rval) = sig * k * ((n + k - 1) / k);
}

static void f_offset (EfiFunc *func, void *rval, void **arg)
{
	EfiObj *base = arg[0];
	EfiObj *obj = arg[1];

	Val_int(rval) = (base && obj) ?
		((char *) obj->data - (char *) base->data) : 0;
}

/*	Funktionsauswertung
*/

static void f_ofunc (EfiFunc *func, void *rval, void **arg)
{
	EfiObjFunc *x;
	EfiObjList *list;
	EfiObj *obj;

	x = arg[0];

	if	(x->obj)
	{
		list = NewObjList(RefObj(x->obj));
		list->next = Val_list(arg[1]);
		obj = EvalFunc(x->func, list);
		list->next = NULL;
		DelObjList(list);
	}
	else	obj = EvalFunc(x->func, Val_list(arg[1]));

	Val_obj(rval) = obj;
}


static void f_func (EfiFunc *func, void *rval, void **arg)
{
	Val_obj(rval) = EvalFunc(Val_func(arg[0]), Val_list(arg[1]));
}

static void f_vfunc (EfiFunc *func, void *rval, void **arg)
{
	Val_obj(rval) = EvalVirFunc(Val_vfunc(arg[0]), Val_list(arg[1]));
}

static void f_tfunc (EfiFunc *func, void *rval, void **arg)
{
	EfiType *type = Val_type(arg[0]);
	EfiObjList *list = Val_list(arg[1]);
	EfiType *x;

	for (x = type; x != NULL; x = x->base)
	{
		EfiVirFunc *ftab = type->create;

		if	(ftab && ftab->tab.used)
		{
			Val_obj(rval) = EvalVirFunc(ftab, list);
			return;
		}
	}

	if	(list == NULL)
	{
		Val_obj(rval) = NewObj(type, NULL);
	}
	else if	(list->next == NULL)
	{
		Val_obj(rval) = EvalObj(RefObj(list->obj), type);
	}
	else
	{
		log_note(NULL, "[efmain:71]", "s", Val_type(arg[0])->name);
	}
}


/*	Sonstiges
*/

static void get_op (EfiFunc *func, void *rval, void **arg)
{
	EfiObj *var;
	EfiObj *obj;

	var = arg[1];
	io_eat(Val_io(arg[0]), "%s");
	obj = Parse_obj(Val_io(arg[0]), SCAN_ANYVAL|SCAN_CHAR|SCAN_STR);
	obj = EvalObj(obj, var->type);

	if	(obj)
	{
		CleanData(var->type, var->data, 0);
		CopyData(var->type, var->data, obj->data);
	}

	Val_io(rval) = rd_refer(Val_io(arg[0]));
}

static void f_putc (EfiFunc *func, void *rval, void **arg)
{
	Val_int(rval) = io_putc(Val_char(arg[0]), Val_io(arg[1]));
}

static void f_puts (EfiFunc *func, void *rval, void **arg)
{
	Val_int(rval) = io_puts(Val_str(arg[0]), Val_io(arg[1]));
}

static void f_printf (EfiFunc *func, void *rval, void **arg)
{
	Val_int(rval) = PrintFmtList(iostd, Val_str(arg[0]),
		Val_list(arg[1]));
}

static void f_fprintf (EfiFunc *func, void *rval, void **arg)
{
	Val_int(rval) = PrintFmtList(Val_io(arg[0]), Val_str(arg[1]),
		Val_list(arg[2]));
}

static void f_sprintf (EfiFunc *func, void *rval, void **arg)
{
	StrBuf *sb;
	IO *io;

	sb = sb_acquire();
	io = io_strbuf(sb);
	PrintFmtList(io, Val_str(arg[0]), Val_list(arg[1]));
	io_close(io);
	Val_str(rval) = sb_cpyrelease(sb);
}

static void f_xprintf (EfiFunc *func, void *rval, void **arg)
{
	StrBuf *sb;

	sb = sb_acquire();
	StrBufFmtList(sb, Val_str(arg[0]), Val_list(arg[1]));
	Val_str(rval) = sb_cpyrelease(sb);
}

static void f_expr (EfiFunc *func, void *rval, void **arg)
{
	Val_obj(rval) = strterm(Val_str(arg[0]));
}

static void f_eval (EfiFunc *func, void *rval, void **arg)
{
	streval(Val_str(arg[0]));
}

static void f_geval (EfiFunc *func, void *rval, void **arg)
{
	PushVarTab(RefVarTab(GlobalVar), NULL);
	streval(Val_str(arg[0]));
	PopVarTab();
}

static void f_cmdeval (EfiFunc *func, void *rval, void **arg)
{
	IO *io = io_cmdpreproc(io_refer(Val_io(arg[0])));

	if	(Val_int(arg[2]))
	{
		PushVarTab(RefVarTab(GlobalVar), NULL);
		CmdEval(io, Val_io(arg[1]));
		PopVarTab();
	}
	else	CmdEval(io, Val_io(arg[1]));

	io_close(io);
}

static void f_ioeval (EfiFunc *func, void *rval, void **arg)
{
	io_eval(Val_io(arg[0]), Val_str(arg[1]));
}

static void f_load (EfiFunc *func, void *rval, void **arg)
{
	IO *io;

	if	((io = io_findopen(Val_str(arg[0]), Val_str(arg[1]),
			Val_str(arg[2]), "rd")))
	{
		io = io_cmdpreproc(io);
		CmdEval(io, NULL);
		io_close(io);
		Val_int(rval) = 1;
	}
	else	Val_int(rval) = 0;
}

static void f_gload (EfiFunc *func, void *rval, void **arg)
{
	IO *io;

	if	((io = io_findopen(Val_str(arg[0]), Val_str(arg[1]),
			Val_str(arg[2]), "rd")))
	{
		io = io_cmdpreproc(io);
		PushVarTab(RefVarTab(GlobalVar), NULL);
		CmdEval(io, NULL);
		PopVarTab();
		io_close(io);
		Val_int(rval) = 1;
	}
	else	Val_int(rval) = 0;
}

static void f_include (EfiFunc *func, void *rval, void **arg)
{
	IO *io;
	Efi *interp;
	char *name;

	interp = Efi_ptr(NULL);
	name = Val_str(arg[0]);

	io = io_cmdpreproc(io_stream(name,
		findopen(IncPath, INC_PFX, name, NULL, "rd"),
		fileclose));

	PushVarTab(RefVarTab(GlobalVar), NULL);
	EfiSrc_hdr(interp, name);
	CmdEval(io, NULL);
	EfiSrc_pop(interp);
	PopVarTab();
	io_close(io);
}

static void f_fsearch (EfiFunc *func, void *rval, void **arg)
{
	Val_str(rval) = fsearch(Val_str(arg[0]), NULL,
		Val_str(arg[1]), Val_str(arg[2]));
}

static void f_texputs (EfiFunc *func, void *rval, void **arg)
{
	Val_int(rval) = TeXputs(Val_str(arg[1]), Val_io(arg[0]));
}

static void f_pcopy (EfiFunc *func, void *rval, void **arg)
{
	Val_int(rval) = io_pcopy(Val_io(arg[0]), Val_io(arg[1]),
		Val_int(arg[2]), 0, NULL);
}

static void f_mpcopy (EfiFunc *func, void *rval, void **arg)
{
	Val_str(rval) = mpcopy(Val_io(arg[0]), Val_int(arg[1]), 0, NULL);
}

static void f_cat (EfiFunc *func, void *rval, void **arg)
{
	StrBuf *sb;
	char *delim;
	EfiObjList *l;
	char *s;

	sb = sb_acquire();
	delim = NULL;

	for (l = Val_list(arg[1]); l != NULL; l = l->next)
	{
		sb_puts(delim, sb);
		delim = Val_str(arg[0]);
		s = Obj2str(RefObj(l->obj));
		sb_puts(s, sb);
		memfree(s);
	}

	Val_str(rval) = sb_cpyrelease(sb);
}

static void f_patcmp (EfiFunc *func, void *rval, void **arg)
{
	char *p;

	if	(func->dim == 3)
		CleanData(&Type_str, arg[2], 0);

	Val_int(rval) = patcmp(Val_str(arg[0]),
		Val_str(arg[1]), func->dim == 3 ? &p : NULL);

	if	(Val_int(rval) && func->dim == 3)
		CopyData(&Type_str, arg[2], &p);
}

static void f_listcmp (EfiFunc *func, void *rval, void **arg)
{
	Val_int(rval) = listcmp(Val_str(arg[0]), Val_char(arg[1]));
}

static void f_pselect (EfiFunc *func, void *rval, void **arg)
{
	char *name, *pat, *p;
	EfiObjList *l;
	int mode, flag;

	flag = 2;
	name = Val_str(arg[0]);

	for (l = Val_list(arg[1]); l != NULL; l = l->next)
	{
		p = pat = Obj2str(RefObj(l->obj));

		if	(!pat)	continue;

		switch (*p)
		{
		case '+':	mode = 1; p++; break;
		case '-':	mode = 0; p++; break;
		default:	mode = 1; break;
		}

		if	(mode != flag)
			flag = (patcmp(p, name, NULL) ? mode : !mode);

		memfree(pat);
	}

	Val_int(rval) = flag ? 1 : 0;
}

static void f_split (EfiFunc *func, void *rval, void **arg)
{
	size_t i, n;
	char **list;
	EfiObjList **ptr;

	n = mstrsplit(Val_str(arg[0]), Val_str(arg[1]), &list);
	ptr = rval;
	*ptr = NULL;

	for (i = 0; i < n; i++)
	{
		*ptr = NewObjList(str2Obj(mstrcpy(list[i])));
		ptr = &(*ptr)->next;
	}

	memfree(list);
}

static void f_index (EfiFunc *func, void *rval, void **arg)
{
	int i, c;
	char *s;

	s = Val_str(arg[0]);
	c = Val_char(arg[1]);
	Val_int(rval) = 0;

	if	(s != NULL)
	{
		for (i = 0; s[i] != 0; i++)
			if (s[i] == c) break;

		Val_int(rval) = i;
	}
}


typedef struct {
	EfiObjList **ptr;
} WPTR;

static int add_entry (const char *name, void *data, void *ptr)
{
	WPTR *wp = ptr;
	*wp->ptr = NewObjList(type2Obj(data));
	wp->ptr = &(*wp->ptr)->next;
	return 0;
}

static void f_typelist (EfiFunc *func, void *rval, void **arg)
{
	extern NameKeyTab TypeTab;
	EfiObjList *list;
	WPTR wp;

	list = NULL;
	wp.ptr = &list;
	nkt_walk(&TypeTab, add_entry, &wp);
	Val_list(rval) = list;
}

static void f_enumlist (EfiFunc *func, void *rval, void **arg)
{
	Val_list(rval) = EnumKeyList(Val_type(arg[0]));
}

static void f_typetree (EfiFunc *func, void *rval, void **arg)
{
	TypeTree(Val_io(arg[1]), Val_type(arg[0]));
}

static void f_typeclass (EfiFunc *func, void *rval, void **arg)
{
	Val_bool(rval) = IsTypeClass(Val_type(arg[0]), Val_type(arg[1]));
}

static void f_setlocale (EfiFunc *func, void *rval, void **arg)
{
	SetLocale(Val_int(arg[0]), Val_str(arg[1]));
}

static void f_setlang (EfiFunc *func, void *rval, void **arg)
{
	SetLangDef(Val_str(arg[0]));
}

static void f_printparam (EfiFunc *func, void *rval, void **arg)
{
	Locale.print = memalloc(sizeof(LCValueDef));
	Locale.print->name = NULL;
	Locale.print->thousands_sep = mstrcpy(Val_str(arg[0]));
	Locale.print->decimal_point = mstrcpy(Val_str(arg[1]));
	Locale.print->negative_sign = mstrcpy(Val_str(arg[2]));
	Locale.print->positive_sign = mstrcpy(Val_str(arg[3]));
	Locale.print->zero_sign = mstrcpy(Val_str(arg[4]));
}

static void f_lcpush (EfiFunc *func, void *rval, void **arg)
{
	PushLocale();
}

static void f_lcpop (EfiFunc *func, void *rval, void **arg)
{
	PopLocale();
}

static void f_pushcontext (EfiFunc *func, void *rval, void **arg)
{
	PushContext(LocalVar, RefObj(LocalObj));
}

static void f_popcontext (EfiFunc *func, void *rval, void **arg)
{
	PopContext();
}

static EfiFuncDef fdef_func[] = {
	{ FUNC_VIRTUAL, &Type_obj, "operator() (Func, List_t)", f_func },
	{ FUNC_VIRTUAL, &Type_obj, "operator() (VirFunc, List_t)", f_vfunc },
	{ FUNC_VIRTUAL, &Type_obj, "operator() (ObjFunc, List_t)", f_ofunc },
	{ FUNC_VIRTUAL, &Type_obj, "operator() (Type_t, List_t)", f_tfunc },

	{ FUNC_VIRTUAL, &Type_obj, "call (Func, ...)", f_func },
	{ FUNC_VIRTUAL, &Type_obj, "call (VirFunc, ...)", f_vfunc },
	{ FUNC_VIRTUAL, &Type_obj, "call (ObjFunc, ...)", f_ofunc },
	{ FUNC_VIRTUAL, &Type_obj, "call (Type_t, ...)", f_tfunc },

	{ FUNC_VIRTUAL, &Type_bool, "isdigit (int)", f_isdigit },
	{ FUNC_VIRTUAL, &Type_bool, "isalpha (int)", f_isalpha },
	{ FUNC_VIRTUAL, &Type_bool, "isalnum (int)", f_isalnum },
	{ FUNC_VIRTUAL, &Type_bool, "isspace (int)", f_isspace },

	{ FUNC_VIRTUAL, &Type_int, "sizeof (.)", c_sizeof },
	{ FUNC_VIRTUAL, &Type_int, "sizeof (_undef_)", u_sizeof },
	{ FUNC_VIRTUAL, &Type_int, "sizeof (Type_t)", t_sizeof },
	{ 0, &Type_str, "whatis (.)", f_whatis },
	{ 0, &Type_str, "declare (Type_t type, bool compact = false)",
		f_declare },
	{ FUNC_VIRTUAL, &Type_int, "offset (., .)", f_offset },
	{ FUNC_VIRTUAL, &Type_int, "align (int, int)", f_align },
	
	{ FUNC_VIRTUAL, &Type_io, "operator>> (IO, . &)", get_op },

	{ 0, &Type_int, "putc (char c, IO io = iostd)", f_putc },
	{ 0, &Type_int, "puts (str s, IO io = iostd)", f_puts },
	{ 0, &Type_int, "printf (str fmt, ...)", f_printf },
	{ 0, &Type_int, "fprintf (IO io, str fmt, ...)", f_fprintf },
	{ 0, &Type_str, "sprintf (str fmt, ...)", f_sprintf },
	{ 0, &Type_str, "xprintf (str fmt, ...)", f_xprintf },

	{ FUNC_VIRTUAL, &Type_int, "pcopy (IO in, IO out, int delim = EOF)",
		f_pcopy },
	{ FUNC_VIRTUAL, &Type_str, "pcopy (IO in, int delim = EOF)", f_mpcopy },
	{ 0, &Type_int, "TeXputs (IO io, str s)", f_texputs },

	{ FUNC_VIRTUAL, &Type_obj, "expr (str expr)", f_expr },
	{ FUNC_VIRTUAL, &Type_void, "eval (str cmd)", f_eval },
	{ FUNC_VIRTUAL, &Type_void, "geval (str cmd)", f_geval },
	{ FUNC_VIRTUAL, &Type_void, "cmdeval (IO in, IO out, bool glob = false)", f_cmdeval },
	{ FUNC_VIRTUAL, &Type_void, "eval (IO io, str delim = NULL)", f_ioeval },
	{ 0, &Type_bool, "load (str path, str name, str type = NULL)", f_load },
	{ 0, &Type_bool, "gload (str path, str name, str type = NULL)", f_gload },
	{ 0, &Type_void, "include (str name)", f_include },
	{ 0, &Type_str, "fsearch (str path, str name, str type = NULL)", f_fsearch },
	{ FUNC_VIRTUAL, &Type_bool, "patcmp (str pat, str s)", f_patcmp },
	{ FUNC_VIRTUAL, &Type_bool, "patcmp (str pat, str s, str &t)", f_patcmp },
	{ FUNC_VIRTUAL, &Type_bool, "pselect (str pat, ...)", f_pselect },
	{ 0, &Type_bool, "listcmp (str pat, char c)", f_listcmp },
	{ 0, &Type_str, "cat (str delim, ...)", f_cat },
	{ 0, &Type_list, "split (str s, str delim = \"%s\")", f_split },
	{ 0, &Type_int, "index (str s, char c)", f_index },

	{ 0, &Type_list, "typelist ()", f_typelist },
	{ 0, &Type_list, "enumlist (Type_t type)", f_enumlist },
	{ 0, &Type_void, "typetree (Type_t type, IO out = iostd)", f_typetree },
	{ 0, &Type_bool, "IsTypeClass (Type_t type, Type_t base = NULL)",
		f_typeclass },
	{ 0, &Type_void, "setlocale (int type, str name = NULL)", f_setlocale },
	{ 0, &Type_void, "setlang (str lang = NULL)", f_setlang },
	{ 0, &Type_void, "lcsave ()", f_lcpush },
	{ 0, &Type_void, "lcrestore ()", f_lcpop },
	{ 0, &Type_void, "printparam (str ts = NULL, str dp = \".\", \
str nsig = \"-\", str psig = \"+\", str zsig = \" \")", f_printparam },
	{ 0, &Type_void, "pushcontext ()", f_pushcontext },
	{ 0, &Type_void, "popcontext ()", f_popcontext },
};


void CmdSetup_func(void)
{
	AddFuncDef(fdef_func, tabsize(fdef_func));
}
