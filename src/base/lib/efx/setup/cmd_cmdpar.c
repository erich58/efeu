/*
Befehlsparameter

$Copyright (C) 2000 Erich Frühstück
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
#include <EFEU/CmdPar.h>
#include <EFEU/RegExp.h>
#include <EFEU/Resource.h>
#include <EFEU/parsub.h>
#include <EFEU/Debug.h>
#include <EFEU/mkpath.h>

#define	E_PROTO	"[efi:proto]$!: Function $1: Prototype not compatible.\n"

extern EfiType Type_StrPool;
EfiType Type_CmdPar = REF_TYPE("CmdPar", CmdPar *);
EfiType Type_ArgList = XREF_TYPE("ArgList", ArgList *, &Type_StrPool);

ArgList *arg_list (const char *base, EfiObjList *list)
{
	if	(base || list)
	{
		ArgList *argl = arg_create();
		arg_cadd(argl, base);

		for (; list != NULL; list = list->next)
			arg_madd(argl, Obj2str(RefObj(list->obj)));

		return argl;
	}
	else	return NULL;
}

ArgList *arg_func (EfiFunc *func, int n, void **arg)
{
	if	(!func || func->dim <= n)
		return NULL;

	if	(func->vaarg)
		return arg_list(NULL, Val_list(arg[func->dim - 1]));

	if	(func->arg[n].type == &Type_ArgList)
		return rd_refer(Val_ptr(arg[n]));

	log_note(NULL, E_PROTO, "s", func->name);
	return NULL;
}

static void f_null (EfiFunc *func, void *rval, void **arg)
{
	Val_ptr(rval) = NULL;
}

static void f_ptr (EfiFunc *func, void *rval, void **arg)
{
	Val_ptr(rval) = rd_refer(CmdPar_ptr(NULL));
}

static void f_create (EfiFunc *func, void *rval, void **arg)
{
	Val_ptr(rval) = CmdPar_alloc(Val_str(arg[0]));
}

static void f_usage (EfiFunc *func, void *rval, void **arg)
{
	CmdPar_usage(Val_ptr(arg[0]), Val_io(arg[1]), Val_str(arg[2]));
}

static void f_iousage (EfiFunc *func, void *rval, void **arg)
{
	CmdPar_iousage(Val_ptr(arg[0]), Val_io(arg[1]), Val_io(arg[2]));
}

static void f_manpage (EfiFunc *func, void *rval, void **arg)
{
	CmdPar_manpage(Val_ptr(arg[0]), Val_io(arg[1]));
}

static void f_read (EfiFunc *func, void *rval, void **arg)
{
	CmdPar *par = rd_refer(Val_ptr(arg[0]));
	CmdPar_read (par, Val_io(arg[1]), EOF, Val_bool(arg[2]));
	Val_ptr(rval) = par;
}

static void f_write (EfiFunc *func, void *rval, void **arg)
{
	CmdPar_write (Val_ptr(arg[0]), Val_io(arg[1]));
}

static void f_load (EfiFunc *func, void *rval, void **arg)
{
	CmdPar_load (Val_ptr(arg[0]), Val_str(arg[1]), 
		Val_bool(arg[2]));
}

static EfiObjList *do_eval (CmdPar *par, EfiObjList *arglist, int flag)
{
	EfiObjList *list, **ptr;
	VecBuf *buf;
	char **argptr;
	int dim;

	buf = vb_create(16, sizeof(char **));

	for (list = arglist; list != NULL; list = list->next)
	{
		argptr = vb_next(buf);
		*argptr = Obj2str(RefObj(list->obj));
	}

	dim = buf->used;
	list = NULL;
	ptr = &list;

	if	(CmdPar_eval(par, &dim, buf->data, flag) >= 0)
	{
		for (argptr = buf->data; dim-- > 0; argptr++)
		{
			*ptr = NewObjList(str2Obj(*argptr));
			ptr = &(*ptr)->next;
			*argptr = NULL;
		}
	}

	vb_clean(buf, memfree);
	vb_free(buf);
	return list;
}

static void f_eval (EfiFunc *func, void *rval, void **arg)
{
	Val_list(rval) = do_eval(Val_ptr(arg[0]), Val_list(arg[1]), 0);
}

static void f_xeval (EfiFunc *func, void *rval, void **arg)
{
	Val_list(rval) = do_eval(Val_ptr(arg[0]), Val_list(arg[1]), 1);
}

static void f_get (EfiFunc *func, void *rval, void **arg)
{
	Val_str(rval) = mstrcpy(CmdPar_getval(Val_ptr(arg[0]),
		Val_str(arg[1]), Val_str(arg[2])));
}

static void f_set (EfiFunc *func, void *rval, void **arg)
{
	CmdPar_setval(Val_ptr(arg[0]),
		Val_str(arg[1]), mstrcpy(Val_str(arg[2])));
}

static void f_setres (EfiFunc *func, void *rval, void **arg)
{
	CmdPar_setval(NULL, Val_str(arg[0]), mstrcpy(Val_str(arg[1])));
}

static void f_getres (EfiFunc *func, void *rval, void **arg)
{
	Val_str(rval) = mstrcpy(CmdPar_getval(NULL,
		Val_str(arg[0]), Val_str(arg[1])));
}

static void f_flagres (EfiFunc *func, void *rval, void **arg)
{
	Val_int(rval) = GetFlagResource(Val_str(arg[0]));
}

static void f_getfmt (EfiFunc *func, void *rval, void **arg)
{
	Val_str(rval) = mstrcpy(GetFormat(Val_str(arg[0])));
}

static EfiObjList *listres (CmdPar *par, RegExp *expr)
{
	EfiObjList *list, **ptr;
	CmdParVar *var;
	size_t n;

	list = NULL;
	ptr = &list;
	par = CmdPar_ptr(par);

	for (n = par->var.used, var = par->var.data; n-- > 0; var++)
	{
		if	(!expr || RegExp_exec (expr, var->name, NULL))
		{
			*ptr = NewObjList(str2Obj(mstrcpy(var->name)));
			ptr = &(*ptr)->next;
		}
	}

	return list;
}

static void f_list (EfiFunc *func, void *rval, void **arg)
{
	Val_list(rval) = listres(Val_ptr(arg[0]), Val_ptr(arg[1]));
}


static void cpar_psub (EfiFunc *func, void *rval, void **arg)
{
	EfiObjList *list;
	ArgList *argl;
	CmdPar *par;
	StrBuf *sb;
	char *fmt;
	IO *out;

	if	((fmt = Val_str(arg[1])) == NULL)
	{
		Val_str(rval) = NULL;
		return;
	}

	par = CmdPar_ptr(Val_ptr(arg[0]));
	argl = arg_create();
	arg_cadd(argl, par->name);

	for (list = Val_list(arg[2]); list != NULL; list = list->next)
	{
		EfiObj *obj = EvalObj(RefObj(list->obj), &Type_str);
		arg_cadd(argl, obj ? Val_str(obj->data) : NULL);
		UnrefObj(obj);
	}

	sb = sb_acquire();
	out = io_strbuf(sb);
	CmdPar_psubout(par, out, fmt, argl);
	io_close(out);
	Val_str(rval) = sb_cpyrelease(sb);
	rd_deref(argl);
}

static void f_listres (EfiFunc *func, void *rval, void **arg)
{
	Val_list(rval) = listres(NULL, Val_ptr(arg[0]));
}

static void f_argl_str (EfiFunc *func, void *rval, void **arg)
{
	Val_ptr(rval) = arg_list(Val_str(arg[0]), NULL);
}

static void f_argl_list (EfiFunc *func, void *rval, void **arg)
{
	Val_ptr(rval) = arg_list(Val_str(arg[0]), Val_list(arg[1]));
}

static void f_argl_add (EfiFunc *func, void *rval, void **arg)
{
	arg_cadd(Val_ptr(arg[0]), Val_str(arg[1]));
}

static void f_argl_set (EfiFunc *func, void *rval, void **arg)
{
	arg_cset(Val_ptr(arg[0]), Val_int(arg[1]), Val_str(arg[2]));
}

static void f_argl_dim (EfiFunc *func, void *rval, void **arg)
{
	ArgList *argl = Val_ptr(arg[0]);
	Val_int(rval) = argl ? argl->dim : 0;
}

static void f_argl_index (EfiFunc *func, void *rval, void **arg)
{
	Val_ptr(rval) = mstrcpy(arg_get(Val_ptr(arg[0]), Val_int(arg[1])));
}

static void f_fmt_show (EfiFunc *func, void *rval, void **arg)
{
	FormatTabShow(Val_io(arg[0]), Val_str(arg[1]), Val_str(arg[2]));
}

static void f_error (EfiFunc *func, void *rval, void **arg)
{
	char *fmt = Val_str(arg[0]);
	ArgList *argl = arg_list(NULL, Val_list(arg[1]));
	log_psub(ErrLog, fmt, argl);
	rd_deref(argl);
	exit(EXIT_FAILURE);
}

static void f_message (EfiFunc *func, void *rval, void **arg)
{
	char *fmt = Val_str(arg[0]);
	ArgList *argl = arg_list(NULL, Val_list(arg[1]));
	log_psub(NoteLog, fmt, argl);
	rd_deref(argl);
}

static void f_ref_debug (EfiFunc *func, void *rval, void **arg)
{
	const RefData *rd = Val_ptr(arg[0]);
	char *fmt = Val_str(arg[1]);

	if	(rd)
	{
		char *id = msprintf("%s(%p %d)", 
			rd->reftype->label ? rd->reftype->label : "",
			rd, rd->refcount);
		ArgList *argl = arg_list(id, Val_list(arg[2]));
		log_psub(rd->reftype->log ? rd->reftype->log : NoteLog,
			fmt, argl);
		rd_deref(argl);
	}
}

static void f_psubfilter (EfiFunc *func, void *rval, void **arg)
{
	IO *io = rd_refer(Val_io(arg[0]));
	ArgList *args = arg_func(func, 1, arg);
	Val_io(rval) = psubfilter(io, args);
}

static void f_psub (EfiFunc *func, void *rval, void **arg)
{
	char *fmt = Val_str(arg[0]);
	ArgList *argl = arg_func(func, 1, arg);
	Val_str(rval) = mpsub(fmt, argl);
	rd_deref(argl);
}

static void f_fpsub (EfiFunc *func, void *rval, void **arg)
{
	IO *io= Val_io(arg[0]);
	char *fmt = Val_str(arg[1]);
	ArgList *argl = arg_func(func, 2, arg);
	Val_int(rval) = io_psub(io, fmt, argl);
	rd_deref(argl);
}

static void f_poolpsub (EfiFunc *func, void *rval, void **arg)
{
	StrPool *pool = Val_ptr(arg[0]);
	IO *io = StrPool_open(pool);
	char *fmt = Val_str(arg[1]);
	ArgList *argl = arg_func(func, 2, arg);
	*((uint64_t *) rval) = StrPool_offset(pool);
	io_psub(io, fmt, argl);
	io_close(io);
	rd_deref(argl);
}

static void f_mkpath (EfiFunc *func, void *rval, void **arg)
{
	Val_str(rval) = mkpath(ProgDir, NULL, Val_str(arg[0]), NULL);
}

/*	Funktionstabelle
*/

static EfiFuncDef func_cpar[] = {
	{ FUNC_RESTRICTED, &Type_CmdPar, "_Ptr_ ()", f_null },
	{ 0, &Type_CmdPar, "CmdPar (void)", f_ptr },
	{ FUNC_RESTRICTED, &Type_CmdPar, "CmdPar (str name)", f_create },
	{ FUNC_VIRTUAL, &Type_void,
		"CmdPar::usage (IO out = NULL, str fmt = NULL)", f_usage },
	{ FUNC_VIRTUAL, &Type_void,
		"CmdPar::usage (IO out, IO def)", f_iousage },
	{ 0, &Type_void, "CmdPar::manpage (IO out = NULL)", f_manpage },
	{ 0, &Type_CmdPar, "CmdPar::read (IO def, bool flag = false)", f_read },
	{ 0, &Type_void, "CmdPar::write (IO def)", f_write },
	{ 0, &Type_void, "CmdPar::load (str name, bool flag = false)", f_load },
	{ 0, &Type_list, "CmdPar::eval (List_t list)", f_eval },
	{ 0, &Type_list, "CmdPar::xeval (List_t list)", f_xeval },
	{ 0, &Type_str, "CmdPar::get (str name, str def = NULL)", f_get },
	{ 0, &Type_void, "CmdPar::set (str name, str val = NULL)", f_set },
	{ 0, &Type_list, "CmdPar::list (RegExp select = NULL)", f_list },
	{ 0, &Type_str, "CmdPar::psub (str fmt, ...)", cpar_psub },
	{ 0, &Type_void, "setres (str name, str val = NULL)", f_setres },
	{ 0, &Type_str, "getres (str name, str def = NULL)", f_getres },
	{ 0, &Type_bool, "flagres (str name)", f_flagres },
	{ 0, &Type_list, "listres (RegExp select = NULL)", f_listres },
	{ 0, &Type_str, "getfmt (str def)", f_getfmt },

	{ FUNC_RESTRICTED, &Type_ArgList, "_Ptr_ ()", f_null },
	{ FUNC_RESTRICTED, &Type_ArgList, "str ()", f_argl_str },
	{ 0, &Type_ArgList, "ArgList (str name, ...)", f_argl_list },
	{ 0, &Type_void, "ArgList::operator+= (str arg)", f_argl_add },
	{ 0, &Type_void, "ArgList::set (int n, str arg = NULL)", f_argl_set },
	{ FUNC_VIRTUAL, &Type_int, "dim (ArgList list)", f_argl_dim },
	{ FUNC_VIRTUAL, &Type_str, "operator[] (ArgList list, int n)",
		f_argl_index },

	{ FUNC_VIRTUAL, &Type_io,
		"psubfilter (promotion IO io)", f_psubfilter },
	{ FUNC_VIRTUAL, &Type_io,
		"psubfilter (promotion IO io, ...)", f_psubfilter },
	{ FUNC_VIRTUAL, &Type_io,
		"psubfilter (promotion IO io, ArgList args)", f_psubfilter },

	{ FUNC_VIRTUAL, &Type_varsize,
		"StrPool::psub (str fmt)", f_poolpsub },
	{ FUNC_VIRTUAL, &Type_varsize,
		"StrPool::psub (str fmt, ...)", f_poolpsub },
	{ FUNC_VIRTUAL, &Type_varsize,
		"StrPool::psub (str fmt, ArgList args)", f_poolpsub },

	{ FUNC_VIRTUAL, &Type_str, "psub (str fmt)", f_psub },
	{ FUNC_VIRTUAL, &Type_str, "psub (str fmt, ...)", f_psub },
	{ FUNC_VIRTUAL, &Type_str, "psub (str fmt, ArgList args)", f_psub },

	{ FUNC_VIRTUAL, &Type_int,
		"fpsub (IO io, str fmt)", f_fpsub },
	{ FUNC_VIRTUAL, &Type_int,
		"fpsub (IO io, str fmt, ...)", f_fpsub },
	{ FUNC_VIRTUAL, &Type_int,
		"fpsub (IO io, str fmt, ArgList args)", f_fpsub },

	{ 0, &Type_void, "message (str fmt, ...)", f_message },
	{ 0, &Type_void, "error (str fmt, ...)", f_error },
	{ 0, &Type_void, "_Ref_::debug (str fmt, ...)", f_ref_debug },
	{ 0, &Type_void, "ShowFmtTab (IO io, str name, str fmt)", f_fmt_show },
	{ 0, &Type_str, "mkpath (str ext)", f_mkpath },
};


void CmdSetup_cmdpar(void)
{
	AddType(&Type_CmdPar);
	AddType(&Type_ArgList);
	AddFuncDef(func_cpar, tabsize(func_cpar));
}
