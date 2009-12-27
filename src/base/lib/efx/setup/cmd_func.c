/*	Standardfunktionen
	(c) 1994 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6

	Version 0.4
*/

#include <EFEU/ftools.h>
#include <EFEU/object.h>
#include <EFEU/cmdconfig.h>
#include <EFEU/cmdeval.h>
#include <EFEU/printobj.h>
#include <EFEU/locale.h>
#include <EFEU/preproc.h>


/*	Die eingebauten Funktionen werden über Makros definiert
*/

#define	BUILTIN(name)	static void name (Func_t *func, void *rval, void **arg)

#define	F_INT(N, E)	BUILTIN(N) { Val_int(rval) = (E); }
#define	F_STR(N, E)	BUILTIN(N) { Val_str(rval) = (E); }


/*	Test auf Variablendefinition
*/

F_INT(c_sizeof, ((Obj_t *) arg[0])->type->size)
F_INT(u_sizeof, 0)
F_INT(t_sizeof, Val_type(arg[0])->size)

CEXPR(c_typeof, Val_type(rval) = ((Obj_t *) arg[0])->type)

BUILTIN(f_whatis)
{
	Obj_t *obj = arg[0];

	if	(obj)
	{
		if	(obj->lref)
		{
			char *p = rd_ident(obj->lref);

			if	(p)
			{
				Val_str(rval) = msprintf("%s & (%s)",
					obj->type->name, p);
				memfree(p);
			}
			else	Val_str(rval) = msprintf("%s &", obj->type->name);
		}
		else	Val_str(rval) = mstrcpy(obj->type->name);
	}
	else	Val_str(rval) = mstrcpy(NULL);

}

BUILTIN(f_align)
{
	register int sig = 1;
	register int n = Val_int(arg[0]);
	register int k = Val_int(arg[1]);

	if	(n == 0 || k == 0)
	{
		Val_int(rval) = n;
		return;
	}

	if	(k < 0)	sig = -sig, k = -k;
	if	(n < 0)	sig = -sig, n = -n;

	Val_int(rval) = sig * k * ((n + k - 1) / k);
}

BUILTIN(f_offset)
{
	register Obj_t *base = arg[0];
	register Obj_t *obj = arg[1];

	Val_int(rval) = (base && obj) ? ((char *) obj->data - (char *) base->data) : 0;
}

/*	Funktionsauswertung
*/

BUILTIN(f_ofunc)
{
	ObjFunc_t *x;
	ObjList_t *list;
	Obj_t *obj;

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


BUILTIN(f_func)
{
	Val_obj(rval) = EvalFunc(Val_func(arg[0]), Val_list(arg[1]));
}

BUILTIN(f_vfunc)
{
	Val_obj(rval) = EvalVirFunc(Val_vfunc(arg[0]), Val_list(arg[1]));
}

BUILTIN(f_tfunc)
{
	Type_t *type = Val_type(arg[0]);
	ObjList_t *list = Val_list(arg[1]);
	Type_t *x;

	for (x = type; x != NULL; x = x->base)
	{
		VirFunc_t *ftab = type->create;

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
		reg_cpy(1, Val_type(arg[0])->name);
		errmsg(MSG_EFMAIN, 71);
	}
}


/*	Sonstiges
*/

BUILTIN(get_op)
{
	Obj_t *var;
	Obj_t *obj;

	var = arg[1];
	io_eat(Val_io(arg[0]), "%s");
	obj = Parse_obj(Val_io(arg[0]), SCAN_ANYVAL|SCAN_CHAR|SCAN_STR);
	obj = EvalObj(obj, var->type);

	if	(obj)
	{
		CleanData(var->type, var->data);
		CopyData(var->type, var->data, obj->data);
	}

	Val_io(rval) = rd_refer(Val_io(arg[0]));
}


BUILTIN(f_printf)
{
	Val_int(rval) = PrintFmtList(iostd, Val_str(arg[0]),
		Val_list(arg[1]));
}

BUILTIN(f_fprintf)
{
	Val_int(rval) = PrintFmtList(Val_io(arg[0]), Val_str(arg[1]),
		Val_list(arg[2]));
}

BUILTIN(f_sprintf)
{
	strbuf_t *sb;
	io_t *io;

	sb = new_strbuf(0);
	io = io_strbuf(sb);
	PrintFmtList(io, Val_str(arg[0]), Val_list(arg[1]));
	io_close(io);
	Val_str(rval) = sb2str(sb);
}


BUILTIN(f_expr)
{
	Val_obj(rval) = strterm(Val_str(arg[0]));
}

BUILTIN(f_eval)
{
	streval(Val_str(arg[0]));
}

BUILTIN(f_geval)
{
	PushVarTab(RefVarTab(GlobalVar), NULL);
	streval(Val_str(arg[0]));
	PopVarTab();
}

BUILTIN(f_cmdeval)
{
	io_t *io = io_cmdpreproc(io_refer(Val_io(arg[0])));

	if	(Val_int(arg[2]))
	{
		PushVarTab(RefVarTab(GlobalVar), NULL);
		CmdEval(io, Val_io(arg[1]));
		PopVarTab();
	}
	else	CmdEval(io, Val_io(arg[1]));

	io_close(io);
}

BUILTIN(f_ioeval)
{
	io_eval(Val_io(arg[0]), Val_str(arg[1]));
}

BUILTIN(f_load)
{
	io_t *io;

	if	((io = io_findopen(Val_str(arg[0]), Val_str(arg[1]), Val_str(arg[2]), "r")))
	{
		io = io_cmdpreproc(io);
		CmdEval(io, NULL);
		io_close(io);
		Val_int(rval) = 1;
	}
	else	Val_int(rval) = 0;
}

BUILTIN(f_gload)
{
	io_t *io;

	if	((io = io_findopen(Val_str(arg[0]), Val_str(arg[1]), Val_str(arg[2]), "r")))
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

F_STR(f_fsearch, fsearch(Val_str(arg[0]), NULL, Val_str(arg[1]), Val_str(arg[2])))

F_STR(f_psub, parsub(Val_str(arg[0])))

BUILTIN(f_vpsub)
{
	ObjList_t *l;
	int i;

	for (i= 1, l = Val_list(arg[1]); l != NULL; i++, l = l->next)
		reg_set(i, Obj2str(RefObj(l->obj)));

	Val_str(rval) = parsub(Val_str(arg[0]));
}

BUILTIN(f_vfpsub)
{
	ObjList_t *l;
	int i;

	for (i= 1, l = Val_list(arg[2]); l != NULL; i++, l = l->next)
		reg_set(i, Obj2str(RefObj(l->obj)));

	Val_int(rval) = io_psub(Val_io(arg[0]), Val_str(arg[1]));
}

BUILTIN(f_iopsub)
{
	fputs("ACHTUNG: psub() anstelle von fpsub().\n", stderr);
	Val_int(rval) = io_psub(Val_io(arg[0]), Val_str(arg[1]));
}

F_INT(f_texputs, TeXputs(Val_str(arg[1]), Val_io(arg[0])))
F_INT(f_pcopy, io_pcopy(Val_io(arg[0]), Val_io(arg[1]), NULL))
F_STR(f_mpcopy, mpcopy(Val_io(arg[0])))

CEXPR(f_errmsg, errmsg(Val_str(arg[0]), Val_int(arg[1])))
CEXPR(f_error, liberror(Val_str(arg[0]), Val_int(arg[1])))
CEXPR(f_message, errmsg(Val_str(arg[0]), Val_int(arg[1])))
F_STR(f_getmsg, mstrcpy(getmsg(Val_str(arg[0]), Val_int(arg[1]), Val_str(arg[2]))))

BUILTIN(f_cat)
{
	strbuf_t *sb;
	char *delim;
	ObjList_t *l;
	char *s;

	sb = new_strbuf(0);
	delim = NULL;

	for (l = Val_list(arg[1]); l != NULL; l = l->next)
	{
		sb_puts(delim, sb);
		delim = Val_str(arg[0]);
		s = Obj2str(RefObj(l->obj));
		sb_puts(s, sb);
		FREE(s);
	}

	Val_str(rval) = sb2str(sb);
}

BUILTIN(f_patcmp)
{
	char *p;

	if	(func->dim == 3)
		CleanData(&Type_str, arg[2]);

	Val_int(rval) = patcmp(Val_str(arg[0]),
		Val_str(arg[1]), func->dim == 3 ? &p : NULL);

	if	(Val_int(rval) && func->dim == 3)
		CopyData(&Type_str, arg[2], &p);
}

BUILTIN(f_listcmp)
{
	Val_int(rval) = listcmp(Val_str(arg[0]), Val_char(arg[1]));
}

BUILTIN(f_pselect)
{
	char *name, *pat, *p;
	ObjList_t *l;
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

BUILTIN(f_split)
{
	size_t i, n;
	char **list;
	ObjList_t **ptr;

	n = strsplit(Val_str(arg[0]), Val_str(arg[1]), &list);
	ptr = rval;
	*ptr = NULL;

	for (i = 0; i < n; i++)
	{
		*ptr = NewObjList(str2Obj(mstrcpy(list[i])));
		ptr = &(*ptr)->next;
	}

	memfree(list);
}

BUILTIN(f_index)
{
	int i, c;
	char *s;

	s = Val_str(arg[0]);
	c = Val_char(arg[1]);
	Val_long(rval) = 0;

	if	(s != NULL)
	{
		for (i = 0; s[i] != 0; i++)
			if (s[i] == c) break;

		Val_int(rval) = i;
	}
}


static void f_typelist (Func_t *func, void *rval, void **arg)
{
	int i;
	ObjList_t *list, **ptr;
	extern xtab_t TypeTab;

	list = NULL;
	ptr = &list;

	for (i = 0; i < TypeTab.dim; i++)
	{
		*ptr = NewObjList(type2Obj(TypeTab.tab[i]));
		ptr = &(*ptr)->next;
	}

	Val_list(rval) = list;
}

static void f_setlocale (Func_t *func, void *rval, void **arg)
{
	if	(!SetLocale(Val_int(arg[0]), Val_str(arg[1])))
	{
		reg_cpy(1, Val_str(arg[1]));
		errmsg(MSG_EFMAIN, 7);
	}
}

static void f_printparam (Func_t *func, void *rval, void **arg)
{
	Locale.print = memalloc(sizeof(LCValue_t));
	Locale.print->name = NULL;
	Locale.print->thousands_sep = mstrcpy(Val_str(arg[0]));
	Locale.print->decimal_point = mstrcpy(Val_str(arg[1]));
	Locale.print->negative_sign = mstrcpy(Val_str(arg[2]));
	Locale.print->positive_sign = mstrcpy(Val_str(arg[3]));
	Locale.print->zero_sign = mstrcpy(Val_str(arg[4]));
}

static void f_getlocale (Func_t *func, void *rval, void **arg)
{
	Val_str(rval) = mstrcpy(GetLocale(Val_int(arg[0])));
}

static void f_lcpush (Func_t *func, void *rval, void **arg)
{
	PushLocale();
}

static void f_lcpop (Func_t *func, void *rval, void **arg)
{
	PopLocale();
}

static void f_pushcontext (Func_t *func, void *rval, void **arg)
{
	PushContext(LocalVar, RefObj(LocalObj));
}

static void f_popcontext (Func_t *func, void *rval, void **arg)
{
	PopContext();
}

static FuncDef_t fdef_func[] = {
	{ FUNC_VIRTUAL, &Type_obj, "operator() (Func, List_t)", f_func },
	{ FUNC_VIRTUAL, &Type_obj, "operator() (VirFunc, List_t)", f_vfunc },
	{ FUNC_VIRTUAL, &Type_obj, "operator() (ObjFunc, List_t)", f_ofunc },
	{ FUNC_VIRTUAL, &Type_obj, "operator() (Type_t, List_t)", f_tfunc },

	{ FUNC_VIRTUAL, &Type_obj, "call (Func, ...)", f_func },
	{ FUNC_VIRTUAL, &Type_obj, "call (VirFunc, ...)", f_vfunc },
	{ FUNC_VIRTUAL, &Type_obj, "call (ObjFunc, ...)", f_ofunc },
	{ FUNC_VIRTUAL, &Type_obj, "call (Type_t, ...)", f_tfunc },

	{ FUNC_VIRTUAL, &Type_int, "sizeof (.)", c_sizeof },
	{ FUNC_VIRTUAL, &Type_int, "sizeof (_undef_)", u_sizeof },
	{ FUNC_VIRTUAL, &Type_int, "sizeof (Type_t)", t_sizeof },
	{ 0, &Type_type, "typeof (.)", c_typeof },
	{ 0, &Type_str, "whatis (.)", f_whatis },
	{ FUNC_VIRTUAL, &Type_int, "offset (., .)", f_offset },
	{ FUNC_VIRTUAL, &Type_int, "align (int, int)", f_align },
	
	{ FUNC_VIRTUAL, &Type_io, "operator>> (IO, . &)", get_op },

	{ 0, &Type_int, "printf (str fmt, ...)", f_printf },
	{ 0, &Type_int, "fprintf (IO io, str fmt, ...)", f_fprintf },
	{ 0, &Type_str, "sprintf (str fmt, ...)", f_sprintf },

	{ FUNC_VIRTUAL, &Type_str, "psub (str fmt)", f_psub },
	{ FUNC_VIRTUAL, &Type_int, "psub (IO io, str fmt)", f_iopsub },
	{ FUNC_VIRTUAL, &Type_str, "psub (str fmt, ...)", f_vpsub },
	{ 0, &Type_int, "fpsub (IO io, str fmt, ...)", f_vfpsub },

	{ FUNC_VIRTUAL, &Type_int, "pcopy (IO in, IO out)", f_pcopy },
	{ FUNC_VIRTUAL, &Type_str, "pcopy (IO in)", f_mpcopy },
	{ 0, &Type_int, "TeXputs (IO io, str s)", f_texputs },

	{ FUNC_VIRTUAL, &Type_obj, "expr (str expr)", f_expr },
	{ FUNC_VIRTUAL, &Type_void, "eval (str cmd)", f_eval },
	{ FUNC_VIRTUAL, &Type_void, "geval (str cmd)", f_geval },
	{ FUNC_VIRTUAL, &Type_void, "cmdeval (IO in, IO out, bool glob = false)", f_cmdeval },
	{ FUNC_VIRTUAL, &Type_void, "eval (IO io, str delim = NULL)", f_ioeval },
	{ 0, &Type_bool, "load (str path, str name, str type = NULL)", f_load },
	{ 0, &Type_bool, "gload (str path, str name, str type = NULL)", f_gload },
	{ 0, &Type_void, "error (str name, int num)", f_error },
	{ 0, &Type_void, "errmsg (str name, int num)", f_errmsg },
	{ 0, &Type_void, "message (str name, int num)", f_message },
	{ 0, &Type_str, "getmsg (str name, int num, str defval = NULL)", f_getmsg },
	{ 0, &Type_str, "fsearch (str path, str name, str type = NULL)", f_fsearch },
	{ FUNC_VIRTUAL, &Type_bool, "patcmp (str pat, str s)", f_patcmp },
	{ FUNC_VIRTUAL, &Type_bool, "patcmp (str pat, str s, str &t)", f_patcmp },
	{ FUNC_VIRTUAL, &Type_bool, "pselect (str pat, ...)", f_pselect },
	{ 0, &Type_bool, "listcmp (str pat, char c)", f_listcmp },
	{ 0, &Type_str, "cat (str delim, ...)", f_cat },
	{ 0, &Type_list, "split (str s, str delim = \"%s\")", f_split },
	{ 0, &Type_int, "index (str s, char c)", f_index },

	{ 0, &Type_list, "typelist ()", f_typelist },
	{ 0, &Type_void, "setlocale (int type, str name = NULL)", f_setlocale },
	{ 0, &Type_str, "getlocale (int type)", f_getlocale },
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
