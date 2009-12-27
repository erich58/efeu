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
#include <EFEU/ArgList.h>

static Type_t Type_cpar = REF_TYPE("CmdPar", CmdPar_t *);
static Type_t Type_argl = REF_TYPE("ArgList", ArgList_t *);

static void f_null (Func_t *func, void *rval, void **arg)
{
	Val_ptr(rval) = NULL;
}

static void f_ptr (Func_t *func, void *rval, void **arg)
{
	Val_ptr(rval) = rd_refer(CmdPar_ptr(NULL));
}

static void f_create (Func_t *func, void *rval, void **arg)
{
	Val_ptr(rval) = CmdPar_alloc(Val_str(arg[0]));
}

static void f_usage (Func_t *func, void *rval, void **arg)
{
	CmdPar_usage(Val_ptr(arg[0]), Val_io(arg[1]), Val_str(arg[2]));
}

static void f_iousage (Func_t *func, void *rval, void **arg)
{
	CmdPar_iousage(Val_ptr(arg[0]), Val_io(arg[1]), Val_io(arg[2]));
}

static void f_manpage (Func_t *func, void *rval, void **arg)
{
	CmdPar_manpage(Val_ptr(arg[0]), Val_io(arg[1]));
}

static void f_read (Func_t *func, void *rval, void **arg)
{
	CmdPar_read (Val_ptr(arg[0]), Val_io(arg[1]), EOF);
}

static void f_write (Func_t *func, void *rval, void **arg)
{
	CmdPar_write (Val_ptr(arg[0]), Val_io(arg[1]));
}

static void f_load (Func_t *func, void *rval, void **arg)
{
	CmdPar_load (Val_ptr(arg[0]), Val_str(arg[1]));
}

static ObjList_t *do_eval (CmdPar_t *par, ObjList_t *arglist, int flag)
{
	ObjList_t *list, **ptr;
	vecbuf_t *buf;
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

static void f_eval (Func_t *func, void *rval, void **arg)
{
	Val_list(rval) = do_eval(Val_ptr(arg[0]), Val_list(arg[1]), 0);
}

static void f_xeval (Func_t *func, void *rval, void **arg)
{
	Val_list(rval) = do_eval(Val_ptr(arg[0]), Val_list(arg[1]), 1);
}

static void f_get (Func_t *func, void *rval, void **arg)
{
	Val_str(rval) = mstrcpy(CmdPar_getval(Val_ptr(arg[0]),
		Val_str(arg[1]), Val_str(arg[2])));
}

static void f_set (Func_t *func, void *rval, void **arg)
{
	CmdPar_setval(Val_ptr(arg[0]),
		Val_str(arg[1]), mstrcpy(Val_str(arg[2])));
}

static void f_setres (Func_t *func, void *rval, void **arg)
{
	CmdPar_setval(NULL, Val_str(arg[0]), mstrcpy(Val_str(arg[1])));
}

static void f_getres (Func_t *func, void *rval, void **arg)
{
	Val_str(rval) = mstrcpy(CmdPar_getval(NULL,
		Val_str(arg[0]), Val_str(arg[1])));
}

static void f_flagres (Func_t *func, void *rval, void **arg)
{
	Val_int(rval) = GetFlagResource(Val_str(arg[0]));
}

static ObjList_t *listres (CmdPar_t *par, RegExp_t *expr)
{
	ObjList_t *list, **ptr;
	CmdParVar_t *var;
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

static void f_list (Func_t *func, void *rval, void **arg)
{
	Val_list(rval) = listres(Val_ptr(arg[0]), Val_ptr(arg[1]));
}

static void f_psub (Func_t *func, void *rval, void **arg)
{
	ObjList_t *list;
	ArgList_t *argl;
	CmdPar_t *par;
	strbuf_t *sb;
	char *fmt;
	io_t *out;

	if	((fmt = Val_str(arg[1])) == NULL)
	{
		Val_str(rval) = NULL;
		return;
	}

	par = CmdPar_ptr(Val_ptr(arg[0]));
	argl = ArgList("c", par->name);

	for (list = Val_list(arg[2]); list != NULL; list = list->next)
		ArgList_madd(argl, Obj2str(RefObj(list->obj)));

	sb = new_strbuf(0);
	out = io_strbuf(sb);
	CmdPar_psubout(par, out, fmt, argl);
	io_close(out);
	Val_str(rval) = sb2str(sb);
	rd_deref(argl);
}

static void f_listres (Func_t *func, void *rval, void **arg)
{
	Val_list(rval) = listres(NULL, Val_ptr(arg[0]));
}

static void f_argl_str (Func_t *func, void *rval, void **arg)
{
	Val_ptr(rval) = ArgList("c", Val_str(arg[0]));
}

static void f_argl_list (Func_t *func, void *rval, void **arg)
{
	ObjList_t *list;
	ArgList_t *argl;

	argl = ArgList("c", Val_str(arg[0]));

	for (list = Val_list(arg[1]); list != NULL; list = list->next)
		ArgList_madd(argl, Obj2str(RefObj(list->obj)));

	Val_ptr(rval) = argl;
}

static void f_argl_add (Func_t *func, void *rval, void **arg)
{
	ArgList_cadd(Val_ptr(arg[0]), Val_str(arg[1]));
}

static void f_argl_dim (Func_t *func, void *rval, void **arg)
{
	ArgList_t *argl = Val_ptr(arg[0]);
	Val_int(rval) = argl ? argl->dim : 0;
}

static void f_argl_index (Func_t *func, void *rval, void **arg)
{
	Val_ptr(rval) = mstrcpy(ArgList_get(Val_ptr(arg[0]), Val_int(arg[1])));
}

/*	Funktionstabelle
*/

static FuncDef_t func_cpar[] = {
	{ FUNC_RESTRICTED, &Type_cpar, "_Ptr_ ()", f_null },
	{ 0, &Type_cpar, "CmdPar (void)", f_ptr },
	{ FUNC_RESTRICTED, &Type_cpar, "CmdPar (str name)", f_create },
	{ FUNC_VIRTUAL, &Type_void,
		"CmdPar::usage (IO out = NULL, str fmt = NULL)", f_usage },
	{ FUNC_VIRTUAL, &Type_void,
		"CmdPar::usage (IO out, IO def)", f_iousage },
	{ 0, &Type_void, "CmdPar::manpage (IO out = NULL)", f_manpage },
	{ 0, &Type_void, "CmdPar::read (IO def)", f_read },
	{ 0, &Type_void, "CmdPar::write (IO def)", f_write },
	{ 0, &Type_void, "CmdPar::load (str name)", f_load },
	{ 0, &Type_list, "CmdPar::eval (List_t list)", f_eval },
	{ 0, &Type_list, "CmdPar::xeval (List_t list)", f_xeval },
	{ 0, &Type_str, "CmdPar::get (str name, str def = NULL)", f_get },
	{ 0, &Type_void, "CmdPar::set (str name, str val = NULL)", f_set },
	{ 0, &Type_list, "CmdPar::list (regex_t select = NULL)", f_list },
	{ 0, &Type_str, "CmdPar::psub (str fmt, ...)", f_psub },
	{ 0, &Type_void, "setres (str name, str val = NULL)", f_setres },
	{ 0, &Type_str, "getres (str name, str def = NULL)", f_getres },
	{ 0, &Type_bool, "flagres (str name)", f_flagres },
	{ 0, &Type_list, "listres (regex_t select = NULL)", f_listres },

	{ FUNC_RESTRICTED, &Type_argl, "_Ptr_ ()", f_null },
	{ FUNC_RESTRICTED, &Type_argl, "str ()", f_argl_str },
	{ 0, &Type_argl, "ArgList (str name, ...)", f_argl_list },
	{ 0, &Type_void, "ArgList::operator+= (str arg)", f_argl_add },
	{ FUNC_VIRTUAL, &Type_int, "dim (ArgList list)", f_argl_dim },
	{ FUNC_VIRTUAL, &Type_str, "operator[] (ArgList list, int n)",
		f_argl_index },
};


void CmdSetup_cmdpar(void)
{
	AddType(&Type_cpar);
	AddType(&Type_argl);
	AddFuncDef(func_cpar, tabsize(func_cpar));
}
