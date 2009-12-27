/*
Funktionen bestimmen und auswerten

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

#include <EFEU/object.h>
#include <EFEU/konvobj.h>

EfiObj *Expr_func (void *par, const EfiObjList *list)
{
	return EvalFunc(par, list);
}


typedef struct {
	EfiFunc *func;
	EfiObj **obj;
	void **arg;
} FUNCPAR;


static void fpar_init(FUNCPAR *par, EfiFunc *func)
{
	par->func = func;
	par->obj = memalloc(func->dim * sizeof(EfiObj *));
	par->arg = memalloc(func->dim * sizeof(void *));
}

static void fpar_clear(FUNCPAR *par, int n)
{
	int i;

	for (i = 0; i < n; i++)
	{
		if	(par->func->arg[i].lval)
			SyncLval(par->obj[i]);

		UnrefObj(par->obj[i]);
	}

	memfree(par->obj);
	memfree(par->arg);
}

static int fpar_error(FUNCPAR *par, const char *fmt, int n)
{
	dbg_note(NULL, "[efmain:60]", NULL);
	ListFunc(ioerr, par->func);
	dbg_note(NULL, fmt, "d", n);
	fpar_clear(par, n);
	return 1;
}

static int test_lval (const EfiObj *obj, const EfiType *type)
{
	const EfiType *old;

	if	(obj == NULL)		return 1;
	if	(obj->lval == NULL)	return 1;
	if	(type == NULL)		return 0;

	for (old = obj->type; old != NULL; old = old->dim ? NULL : old->base)
		if (old == type) return 0;

	return 1;
}


static int fpar_set(FUNCPAR *par, int n, EfiObj *obj)
{
	register EfiFuncArg *arg = par->func->arg + n;

	if	(arg->lval)
	{
		par->obj[n] = EvalObj(RefObj(obj), NULL);

		if	(test_lval(par->obj[n], arg->type))
			return fpar_error(par, "[efmain:62]", n);
	}
	else
	{
		par->obj[n] = EvalObj(RefObj(obj), arg->type);

		if	(par->obj[n] == NULL)
			return fpar_error(par, "[efmain:61]", n);
	}

	par->arg[n] = arg->type ? par->obj[n]->data : par->obj[n];
	return 0;
}

#define	listtype(type)	((type) == &Type_list || (type) == &Type_explist)
#define	is_list(obj)	((obj) && listtype((obj)->type))

EfiObj *EvalFunc (EfiFunc *func, const EfiObjList *list)
{
	FUNCPAR par;
	EfiObj *obj, *firstarg;
	int i, dim;

	if	(func == NULL)
		return NULL;

	if	(IsVirFunc(func))
		return EvalVirFunc((void *) func, list);

	if	(!IsFunc(func))
	{
		dbg_error(NULL, "[efmain:65]", NULL);
		return NULL;
	}

	FuncDebug(func, "eval");
	
	if	(func->dim == 0)
	{
		if	(list != NULL)
		{
			fpar_init(&par, func);
			fpar_error(&par, "[efmain:63]", 0);
			return NULL;
		}

		return MakeRetVal(func, NULL, NULL);
	}

	fpar_init(&par, func);
	dim = func->dim - func->vaarg;
	firstarg = list ? list->obj : NULL;

	for (i = 0; i < dim; i++)
	{
		if	(list)
		{
			obj = list->obj;
			list = list->next;
		}
		else	obj = func->arg[i].defval;

		if	(fpar_set(&par, i, obj))
			return NULL;
	}

	if	(func->vaarg)
	{
		list = EvalObjList(list);
		par.arg[dim] = (list && !list->next && is_list(list->obj)) ?
			list->obj->data : &list;
	}
	else if	(list != NULL)
	{
		fpar_error(&par, "[efmain:63]", dim);
		return NULL;
	}

	obj = MakeRetVal(func, firstarg, par.arg);
	fpar_clear(&par, dim);
	DelObjList((EfiObjList *) list);
	return obj;
}
