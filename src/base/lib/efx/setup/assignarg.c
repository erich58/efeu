/*
Zuweisungsargument

$Copyright (C) 1997 Erich Frühstück
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
#include <EFEU/printobj.h>
#include <EFEU/locale.h>
#include <EFEU/preproc.h>
#include <EFEU/pconfig.h>
#include <EFEU/parsearg.h>
#include <EFEU/stdtype.h>

static void t_clean (const EfiType *st, void *data, int mode)
{
	memfree(Val_ptr(data));
	Val_ptr(data) = NULL;
}

static void t_copy (const EfiType *st, void *tg, const void *src)
{
	Val_ptr(tg) = NULL;
}

EfiType Type_assignarg = PTR_TYPE("AssignArg", AssignArg *,
	&Type_ptr, NULL, t_clean, t_copy);

static EfiObj *m_name (const EfiObj *base, void *data)
{
	AssignArg *arg = base ? Val_ptr(base->data) : NULL;
	return str2Obj(base ? mstrcpy(arg->name) : NULL);
}

static EfiObj *m_opt (const EfiObj *base, void *data)
{
	AssignArg *arg = base ? Val_ptr(base->data) : NULL;
	return str2Obj(base ? mstrcpy(arg->opt) : NULL);
}

static EfiObj *m_arg (const EfiObj *base, void *data)
{
	AssignArg *arg = base ? Val_ptr(base->data) : NULL;
	return str2Obj(base ? mstrcpy(arg->arg) : NULL);
}

static EfiMember vtab[] = {
	{ "name", &Type_str, m_name, NULL },
	{ "opt", &Type_str, m_opt, NULL },
	{ "arg", &Type_str, m_arg, NULL },
};

static void f_create (EfiFunc *func, void *rval, void **arg)
{
	Val_ptr(rval) = assignarg(Val_str(arg[0]), NULL, Val_str(arg[1]));
}

static void f_xcreate (EfiFunc *func, void *rval, void **arg)
{
	char *def, *p;

	def = Val_str(arg[0]);
	Val_ptr(rval) = assignarg(def, &p, Val_str(arg[1]));
	Val_str(arg[0]) = mstrcpy(p);
	memfree(def);
}

static void f_fprint (EfiFunc *func, void *rval, void **arg)
{
	IO *io = Val_io(arg[0]);
	AssignArg *x = Val_ptr(arg[1]);
	size_t n;

	if	(x == NULL)
	{
		Val_int(rval) = io_puts("NULL", io);
		return;
	}

	n = io_puts(x->name, io);

	if	(x->opt)
	{
		n += io_puts("[", io);
		n += io_puts(x->opt, io);
		n += io_puts("]", io);
	}

	if	(x->arg)
	{
		n += io_puts("=", io);
		n += io_puts(x->arg, io);
	}

	Val_int(rval) = n;
}

static void f_list (EfiFunc *func, void *rval, void **arg)
{
	AssignArg *x = Val_ptr(arg[0]);
	EfiObjList *list = NULL;
	
	if	(x)
	{
		list = NewObjList(str2Obj(mstrcpy(x->name)));
		list->next = NewObjList(str2Obj(mstrcpy(x->opt)));
		list->next->next = NewObjList(str2Obj(mstrcpy(x->arg)));
	}

	Val_list(rval) = list;
}

static EfiFuncDef ftab[] = {
	{ 0, &Type_list, "AssignArg ()", f_list },
	{ 0, &Type_assignarg, "AssignArg (str def, str delim = NULL)",
		f_create },
	{ 0, &Type_assignarg, "AssignArg (str & s, str delim = NULL)",
		f_xcreate },
	{ FUNC_VIRTUAL, &Type_int, "fprint (IO, AssignArg)", f_fprint },
};

void CmdSetup_assignarg(void)
{
	AddType(&Type_assignarg);
	AddFuncDef(ftab, tabsize(ftab));
	AddEfiMember(Type_assignarg.vtab, vtab, tabsize(vtab));
}
