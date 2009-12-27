/*
Funktionen auswerten},

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


/*	Funktionsaufrufe
*/

void Func_func (Func_t *func, void *rval, void **arg)
{
	VarStack_t *vstack = SaveVarStack();
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


void Func_inline (Func_t *func, void *rval, void **arg)
{
	Obj_t *x;
	Var_t *var;
	VarTab_t *vtab;
	int i;

/*	Variablentabelle für Argumente aufbauen
*/
	if	(func->dim)
	{
		var = ALLOC(func->dim, Var_t);
		memset(var, 0, func->dim * sizeof(Var_t));

		for (i = 0; i < func->dim; i++)
		{
			if	(func->arg[i].name == NULL)	continue;

			var[i].name = func->arg[i].name;
			var[i].dim = 0;

			if	(func->arg[i].type == &Type_vec)
			{
				Vec_t *vec;

				vec = arg[i];
				var[i].type = vec->type;
				var[i].data = vec->data;
				var[i].dim = vec->dim;
			}
			else if	(func->arg[i].type == NULL)
			{
				var[i].type = &Type_obj;
				var[i].data = &arg[i];
			}
			else if	(func->arg[i].lval == 0
				&& func->arg[i].cnst == 0)
			{
				var[i].type = func->arg[i].type;
				var[i].data = memalloc(var[i].type->size);
				CopyData(var[i].type, var[i].data, arg[i]);
			}
			else if	(func->arg[i].type)
			{
				var[i].type = func->arg[i].type;
				var[i].data = arg[i];
			}
		}

		vtab = VarTab(func->name, func->dim);
		AddVar(vtab, var, func->dim);

		if	(func->scope)
		{
			x = func->bound ? Var2Obj(var, NULL) : NULL;
			PushVarTab(RefVarTab(func->scope), x);
		}

		PushVarTab(vtab, NULL);
	}
	else	var = NULL;

	x = EvalExpression(func->par);

/*	Rückgabewert ermitteln
*/
	if	(func->type == NULL)
	{
		Val_obj(rval) = x;
	}
	else if	(func->lretval)
	{
		UnrefObj(x);
		x = NULL;
	}
	else	Obj2Data(x, func->type, rval);

/*	Variablentabelle für Argumente löschen
*/
	if	(func->dim)
	{
		for (i = 0; i < func->dim; i++)
		{
			if	(func->arg[i].name == NULL)	continue;
			if	(func->arg[i].type == &Type_vec) continue;
			if	(func->arg[i].type == NULL)	continue;
			if	(func->arg[i].lval != 0)	continue;
			if	(func->arg[i].cnst != 0)	continue;

			CleanData(var[i].type, var[i].data);
			memfree(var[i].data);
		}

		PopVarTab();
		memfree(var);

		if	(func->scope)
			PopVarTab();
	}
}
