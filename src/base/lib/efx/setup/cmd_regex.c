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
#include <EFEU/Op.h>

static EfiType Type_regex = REF_TYPE("RegExp", RegExp *);

static void regex2str (EfiFunc *func, void *rval, void **arg)
{
	RegExp *x = Val_ptr(arg[0]);
	Val_ptr(rval) = x ? mstrcpy(x->def) : NULL;
}

static void str2regex (EfiFunc *func, void *rval, void **arg)
{
	Val_ptr(rval) = RegExp_comp(Val_str(arg[0]), 0);
}

static EfiObj *parse_regex(IO *io, EfiOp *op, EfiObj *left)
{
	StrBuf *sb;
	EfiObj *obj;
	int c;
	int flag;

	sb = sb_acquire();
	io_protect(io, 1);

	while ((c = io_getc(io)) != EOF)
	{
		if	(c == '\\')
		{
			c = io_getc(io);

			if	(c != '/')
				sb_putc('\\', sb);

			if	(c == EOF)	break;
		}
		else if	(c == '/')	break;

		sb_putc(c, sb);
	}

	io_protect(io, 0);

	switch (io_peek(io))
	{
	case 'i':
	case 'I':
		io_getc(io);
		flag = 1;
		break;
	default:
		flag = 0;
		break;
	}

	obj = NewPtrObj(&Type_regex, RegExp_comp(sb_nul(sb), flag));
	sb_release(sb);
	return obj;
}

static EfiOp op_regex[] = {
	{ "/", OpPrior_Unary, OpAssoc_Right, parse_regex },
};

static void f_regex (EfiFunc *func, void *rval, void **arg)
{
	Val_ptr(rval) = RegExp_comp(Val_str(arg[0]), Val_int(arg[1]));
}

static int sub_cmp (RegExp *a, RegExp *b)
{
	if	(a == b)	return 1;
	else if	(!a || !b)	return 0;
	else if	(a->icase != b->icase)	return 0;
	else if	(mstrcmp(a->def, b->def) == 0)	return 1;
	else			return 0;
}

static void f_regcmp (EfiFunc *func, void *rval, void **arg)
{
	Val_int(rval) = sub_cmp (Val_ptr(arg[0]), Val_ptr(arg[1]));
}

static void f_nregcmp (EfiFunc *func, void *rval, void **arg)
{
	Val_int(rval) = sub_cmp (Val_ptr(arg[0]), Val_ptr(arg[1]));
}

static void f_regexec (EfiFunc *func, void *rval, void **arg)
{
	Val_int(rval) = RegExp_exec(Val_ptr(arg[0]),
		Val_str(arg[1]), NULL);
}

static void f_iregexec (EfiFunc *func, void *rval, void **arg)
{
	Val_int(rval) = RegExp_exec(Val_ptr(arg[1]),
		Val_str(arg[0]), NULL);
}

static void f_nregexec (EfiFunc *func, void *rval, void **arg)
{
	Val_int(rval) = ! RegExp_exec(Val_ptr(arg[0]),
		Val_str(arg[1]), NULL);
}

static void f_niregexec (EfiFunc *func, void *rval, void **arg)
{
	Val_int(rval) = ! RegExp_exec(Val_ptr(arg[1]),
		Val_str(arg[0]), NULL);
}

static void f_regsub (EfiFunc *func, void *rval, void **arg)
{
	Val_str(rval) = RegExp_subst(Val_ptr(arg[0]),
		Val_str(arg[1]), Val_str(arg[2]), Val_int(arg[3]));
}

static void f_match (EfiFunc *func, void *rval, void **arg)
{
	regmatch_t *match;
	char *str;
	int n;

	str = Val_str(arg[0]);
	n = RegExp_exec(Val_ptr(arg[1]), str, &match);

	if	(n)
	{
		n--;
		Val_str(rval) = mstrncpy(str + match[n].rm_so,
			match->rm_eo - match[n].rm_so);
	}
	else	Val_str(rval) = NULL;
}

static EfiObjList *match_to_list(const char *str, regmatch_t *match, size_t n)
{
	EfiObjList *list, **ptr;

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

static void f_regmatch (EfiFunc *func, void *rval, void **arg)
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

static EfiFuncDef func_regex[] = {
	{ FUNC_RESTRICTED, &Type_str, "RegExp ()", regex2str },
	{ 0, &Type_regex, "RegExp (str expr)", str2regex },
	{ 0, &Type_regex, "RegExp (str expr, bool icase = false)", f_regex },
	{ 0, &Type_bool, "RegExp::exec (str s)", f_regexec },
	{ 0, &Type_list, "RegExp::match (str s)", f_regmatch },
	{ 0, &Type_str, "RegExp::sub (str repl, str s, bool glob = false)",
		f_regsub },
	{ 0, &Type_bool, "regexec (RegExp re, str s)", f_regexec },
	{ FUNC_VIRTUAL, &Type_bool,
		"operator== (restricted RegExp a, restricted RegExp b)",
		f_regcmp },
	{ FUNC_VIRTUAL, &Type_bool,
		"operator== (restricted RegExp re, promotion str s)",
		f_regexec },
	{ FUNC_VIRTUAL, &Type_bool,
		"operator== (promotion str s, restricted RegExp re)",
		f_iregexec },
	{ FUNC_VIRTUAL, &Type_bool,
		"operator!= (restricted RegExp a, restricted RegExp b)",
		f_nregcmp },
	{ FUNC_VIRTUAL, &Type_bool,
		"operator!= (restricted RegExp re, promotion str s)",
		f_nregexec },
	{ FUNC_VIRTUAL, &Type_bool,
		"operator!= (promotion str s, restricted RegExp re)",
		f_niregexec },
	{ FUNC_VIRTUAL, &Type_str,
		"operator: (promotion str s, restricted RegExp re)",
		f_match },
	{ 0, &Type_list, "regmatch (RegExp re, str s)", f_regmatch },
	{ 0, &Type_str, "regsub (RegExp re, str r, str s, bool glob = false)",
		f_regsub },
};


void CmdSetup_regex(void)
{
	AddType(&Type_regex);
	AddOpDef(&PrefixTab, op_regex, tabsize(op_regex));
	AddFuncDef(func_regex, tabsize(func_regex));
}
