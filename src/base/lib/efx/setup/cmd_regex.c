/*
Reguläre Ausdrücke

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
#include <EFEU/RegExp.h>

static Type_t Type_regex = REF_TYPE("regex_t", RegExp_t *);

static void str2regex (Func_t *func, void *rval, void **arg)
{
	Val_ptr(rval) = RegExp(Val_str(arg[0]), 0);
}

static void f_regex (Func_t *func, void *rval, void **arg)
{
	Val_ptr(rval) = RegExp(Val_str(arg[0]), Val_int(arg[1]));
}

static void f_regexec (Func_t *func, void *rval, void **arg)
{
	Val_int(rval) = RegExp_exec(Val_ptr(arg[0]),
		Val_str(arg[1]), NULL);
}

static void f_regsub (Func_t *func, void *rval, void **arg)
{
	Val_str(rval) = RegExp_subst(Val_ptr(arg[0]),
		Val_str(arg[1]), Val_str(arg[2]), Val_int(arg[3]));
}

static ObjList_t *match_to_list(const char *str, regmatch_t *match, size_t n)
{
	ObjList_t *list, **ptr;

	if	(!match)
		return NewObjList(str2Obj(NULL));

	list = NULL;
	ptr = &list;

	for (; n-- > 0; match++)
	{
		*ptr = NewObjList(str2Obj(mstrncpy(str + match->rm_so,
			match->rm_eo - match->rm_so)));
		ptr = &(*ptr)->next;
	}

	return list;
}

static void f_regmatch (Func_t *func, void *rval, void **arg)
{
	regmatch_t *match;
	char *str;
	int n;

	str = Val_str(arg[1]);
	n = RegExp_exec(Val_ptr(arg[0]), str, &match);
	Val_list(rval) = n ? match_to_list(str, match, n) : NULL;
}

/*	Funktionstabelle
*/

static FuncDef_t func_regex[] = {
	{ 0, &Type_regex, "regex_t (str expr)", str2regex },
	{ 0, &Type_regex, "regex_t (str expr, bool icase = false)", f_regex },
	{ 0, &Type_bool, "regex_t::exec (str s)", f_regexec },
	{ 0, &Type_list, "regex_t::match (str s)", f_regmatch },
	{ 0, &Type_str, "regex_t::sub (str repl, str s, bool glob = false)",
		f_regsub },
	{ 0, &Type_bool, "regexec (regex_t re, str s)", f_regexec },
	{ 0, &Type_list, "regmatch (regex_t re, str s)", f_regmatch },
	{ 0, &Type_str, "regsub (regex_t re, str r, str s, bool glob = false)",
		f_regsub },
};


void CmdSetup_regex(void)
{
	AddType(&Type_regex);
	AddFuncDef(func_regex, tabsize(func_regex));
}
