/*	Funktionen auswerten
	(c) 1994 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6

	Version 0.4
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

/*	Rückgabewewert ermitteln
*/
	if	(func->type == NULL)
	{
		Val_obj(rval) = x;
	}
	else if	(func->lretval)
	{
		if	(!x || !x->lref || x->type != func->type)
		{
			reg_cpy(1, func->name);
			errmsg(MSG_EFMAIN, 175);
		}
		else	Val_ptr(rval) = x->data;

		UnrefObj(x);
	}
	else	Obj2Data(x, func->type, rval);

/*	Variablentabelle für Argumente Löschen
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
