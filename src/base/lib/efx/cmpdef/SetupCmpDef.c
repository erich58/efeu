/*
:*:	setup compare definition
:de:	Initialisierung der Vergleichsdefinitionen

$Copyright (C) 2005 Erich Frühstück
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

#include <EFEU/CmpDef.h>
#include <EFEU/stdtype.h>

EfiType Type_CmpDef = REF_TYPE("CmpDef", CmpDef *);

static int do_cmp (CmpDef *cmp, EfiObj *a, EfiObj *b)
{
	if	(a == b)	return 0;
	if	(a == NULL)	return -1;
	if	(b == NULL)	return 1;

	return cmp_data(cmp, a->data, b->data);
}

static void f_create (EfiFunc *func, void *rval, void **arg)
{
	Val_ptr(rval) = cmp_create(Val_ptr(arg[0]),
		func->dim > 1 ? Val_str(arg[1]) : NULL, NULL);
}

static void f_cmp (EfiFunc *func, void *rval, void **arg)
{
	CmpDef *cmp = Val_ptr(arg[0]);
	EfiObjList *list = Val_list(arg[1]);

	if	(!cmp || !list || !list->next) 
	{
		Val_int(rval) = 0;
	}
	else if	(list->next->next)
	{
		EfiObj *a = EvalObj(RefObj(list->obj), cmp->type);
		EfiObj *x = EvalObj(RefObj(list->next->obj), cmp->type);
		EfiObj *b = EvalObj(RefObj(list->next->next->obj), cmp->type);

		if	(do_cmp(cmp, x, a) < 0)	Val_int(rval) = -1;
		else if	(do_cmp(cmp, x, b) > 0)	Val_int(rval) = 1;
		else				Val_int(rval) = 0;

		UnrefObj(a);
		UnrefObj(x);
		UnrefObj(b);
	}
	else
	{
		EfiObj *a = EvalObj(RefObj(list->obj), cmp->type);
		EfiObj *b = EvalObj(RefObj(list->next->obj), cmp->type);
		Val_int(rval) = do_cmp(cmp, a, b);
		UnrefObj(a);
		UnrefObj(b);
	}
}

static EfiFuncDef fdef[] = {
	{ 0, &Type_CmpDef, "CmpDef (Type_t type)", f_create },
	{ 0, &Type_CmpDef, "CmpDef (Type_t type, str def)", f_create },
	{ FUNC_VIRTUAL, &Type_int, "operator() (CmpDef, List_t)", f_cmp },
};

void SetupCmpDef()
{
	static int setup_done = 0;

	if	(setup_done)	return;

	setup_done = 1;

	AddType(&Type_CmpDef);
	AddFuncDef(fdef, tabsize(fdef));
}
