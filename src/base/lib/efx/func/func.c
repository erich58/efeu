/*
Funktionen auswerten

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
#include <EFEU/cmdeval.h>


/*	Funktionsaufrufe
*/

void Func_func (EfiFunc *func, void *rval, void **arg)
{
	EfiVarStack *vstack = SaveVarStack();
	LocalVar = RefVarTab(GlobalVar);

	if	(ContextVar)
	{
		PushVarTab(RefVarTab(ContextVar), RefObj(ContextObj));
		Func_inline(func, rval, arg);
		PopVarTab();
	}
	else	Func_inline(func, rval, arg);

	RestoreVarStack(vstack);
}

static EfiObj *farg_obj (EfiFuncArg *arg, void *data)
{
	if	(arg->type == NULL)
		return RefObj(data);

	if	(arg->lval || arg->cnst)
		return LvalObj(&Lval_ptr, arg->type, data);

	return LvalObj(&Lval_xdata, arg->type, data);
}

void Func_inline (EfiFunc *func, void *rval, void **arg)
{
	EfiObj *x;
	EfiVarTab *vtab;
	VarTabEntry *entry;
	int rbase;
	int i;

	rbase = (!func->bound && !func->name);

	if	(rbase)
	{
		EfiObj *base = LvalObj(&Lval_ptr, func->type, rval);
		PushVarTab(RefVarTab(func->type->vtab), base);
	}

/*	Variablentabelle für Argumente aufbauen
*/
	if	(func->dim)
	{
		vtab = VarTab(func->name, func->dim);

		for (i = 0; i < func->dim; i++)
		{
			if	(func->arg[i].name == NULL)
				continue;

			entry = VarTab_next(vtab);
			entry->name = func->arg[i].name;
			entry->type = func->arg[i].type;
			entry->desc = NULL;
			entry->obj = farg_obj(func->arg + i, arg[i]);
			entry->get = NULL;
			entry->data = NULL;
			entry->entry_clean = NULL;
		}

		if	(func->scope)
		{
			if	(func->bound)
			{
				entry = vtab->tab.data;
				x = RefObj(entry->obj);
			}
			else	x = NULL;

			PushVarTab(RefVarTab(func->scope), x);
		}

		VarTab_qsort(vtab);
		PushVarTab(vtab, NULL);
	}

	if	((x = EvalExpression(func->par)))
	{
		if	(func->type == NULL)
		{
			Val_obj(rval) = x;
		}
		else if	(func->lretval)
		{
			UnrefObj(x);
		}
		else	Obj2Data(x, func->type, rval);
	}

/*	Variablentabelle für Argumente löschen
*/
	if	(func->dim)
	{
		PopVarTab();

		if	(func->scope)
			PopVarTab();
	}

	if	(rbase)
		PopVarTab();
}
