/*
Klassifikationsausdruck generieren

$Copyright (C) 2007 Erich Frühstück
This file is part of EFEU.

EFEU is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public
License as published by the Free Software Foundation; either
version 2 of the License, or (at your option) any later version.

EFEU is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty
of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU General Public License for more details.

You should have received a copy of the GNU General Public
License along with EFEU; see the file COPYING.
If not, write to the Free Software Foundation, Inc.,
59 Temple Place, Suite 330, Boston, MA 02111-1307, USA.
*/

#include <EFEU/EfiClass.h>
#include <EFEU/parsearg.h>

static EfiObj *class_eval (void *par, const EfiObjList *list)
{
	int (*update) (const EfiObj *obj, void *par) = par;
	EfiType *type = Val_type(list->next->obj->data);
	int val = update(EvalObj(RefObj(list->obj), NULL),
		Val_ptr(list->next->next->obj->data));
	return NewObj(type, &val);
}

static EfiObj *class_expr (void *par, const EfiObjList *list)
{
	int (*update) (const EfiObj *obj, void *par) = par;
	EfiType *type = Val_type(list->next->obj->data);
	int val = update(EvalExpression(RefObj(list->obj)),
		Val_ptr(list->next->next->obj->data));
	return NewObj(type, &val);
}

EfiObj *EfiClassExpr (EfiObj *base, const char *def)
{
	AssignArg *x;
	EfiClass *cl;
	EfiObj *(*cfunc) (void *par, const EfiObjList *list);
	EfiObj *obj;

	if	((obj = EvalObj(RefObj(base), NULL)))
		cfunc = class_eval;
	else if	((obj = EvalExpression(RefObj(base))))
		cfunc = class_expr;
	else	return base;

	if	(def == NULL || *def == 0)
	{
		ListEfiPar(ioerr, obj->type, &EfiPar_class, NULL, 0);
		exit(EXIT_SUCCESS);
	}

	if	(*def == '?')
	{
		ListEfiPar(ioerr, obj->type, &EfiPar_class, NULL,
			def[1] == '?' ? 2 : 1);
		exit(EXIT_SUCCESS);
	}

	x = assignarg(def, NULL, NULL);
	cl = SearchEfiPar(obj->type, &EfiPar_class, x->name);

	if	(!cl)
	{
		;
	}
	else if	(x->arg && x->arg[0] == '?')
	{
		PrintEfiPar(ioerr, cl);
		exit(EXIT_SUCCESS);
	}
	else if	(cl->create)
	{
		EfiClassArg arg;
		cl->create(&arg, x->opt, x->arg);
		base = Obj_call(class_expr, arg.update, MakeObjList(3, base,
			NewPtrObj(&Type_type, arg.type),
			NewPtrObj(&Type_ref, arg.par)));
	}

	memfree(x);
	UnrefObj(obj);
	return base;
}
