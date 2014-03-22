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
#include <EFEU/printobj.h>

static EfiObj *farg_ptr (EfiType *type, va_list list)
{
	EfiObj *obj = Obj_alloc(sizeof *obj);
	obj->data = va_arg(list, void *);
	return obj;
}

static EfiObj *farg_copy (EfiType *type, va_list list)
{
	EfiObj *obj = Obj_alloc(sizeof *obj + type->size);
	obj->data = obj + 1;
	CopyData(type, obj->data, va_arg(list, void *));
	return obj;
}

static char *farg_ident (const EfiObj *obj)
{
	return msprintf("%s %p", obj->lval->name, obj->data);
}

static void farg_keep (EfiObj *obj)
{
	;
}

static EfiLval farg_lval = {
	"farg_lval", NULL,
	farg_ptr, farg_keep,	/* alloc, free */
	NULL, NULL, NULL,	/* update, sync, unlink */
	farg_ident,
};

static EfiLval farg_data = {
	"farg_data", NULL,
	farg_copy, NULL,
	NULL, NULL, NULL,
	farg_ident,
};


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

static void check_use (VarTabEntry *entry)
{
	if	(entry->obj->refcount)
		log_error(NULL, "[efmain:169]", "s", entry->name);
}

static void farg_set (EfiVarTab *vtab, EfiFuncArg *arg, void *data, int bound)
{
	VarTabEntry *entry = VarTab_next(vtab);
	entry->name = arg->name;
	entry->type = arg->type;
	entry->desc = NULL;
	entry->get = NULL;
	entry->data = NULL;
	entry->entry_clean = NULL;

	if	(!arg->type)
	{
		entry->obj = RefObj(data);
	}
	else if	(arg->lval)
	{
		entry->obj = LvalObj(&farg_lval, arg->type, data);
		entry->entry_clean = bound ? NULL : check_use;
	}
	else if	(arg->cnst)
	{
		if	(bound)
		{
			entry->obj = LvalObj(&farg_lval, arg->type, data);
		}
		else
		{
			entry->obj = ExtObj(arg->type, data);
			entry->entry_clean = check_use;
		}
	}
	else	entry->obj = LvalObj(&farg_data, arg->type, data);
}

void Func_inline (EfiFunc *func, void *rval, void **arg)
{
	EfiObj *obj;
	EfiVarTab *vtab;
	VarTabEntry *entry;
	int rbase;
	int bound;
	int i;

	rbase = (!func->bound && !func->name);

	if	(rbase)
	{
		EfiObj *base = LvalObj(&farg_lval, func->type, rval);
		PushVarTab(RefVarTab(func->type->vtab), base);
	}

/*	Variablentabelle für Argumente aufbauen
*/
	if	(func->dim)
	{
		vtab = VarTab(func->name, func->dim);

		for (i = 0, bound = func->bound; i < func->dim; i++, bound = 0)
			if (func->arg[i].name)
				farg_set(vtab, func->arg + i, arg[i], bound);

		if	(func->scope)
		{
			if	(func->bound)
			{
				entry = vtab->tab.data;
				obj = RefObj(entry->obj);
			}
			else	obj = NULL;

			PushVarTab(RefVarTab(func->scope), obj);
		}

		VarTab_qsort(vtab);
		PushVarTab(vtab, NULL);
	}
	else	vtab = NULL;

	if	((obj = EvalExpression(func->par)))
	{
		if	(func->type == NULL)
		{
			Val_obj(rval) = obj;
		}
		else if	(func->lretval)
		{
			UnrefObj(obj);
		}
		else	Obj2Data(obj, func->type, rval);
	}

/*	Variablentabelle für Argumente löschen
*/
	if	(vtab)
	{
		PopVarTab();

		if	(func->scope)
			PopVarTab();
	}

	if	(rbase)
		PopVarTab();
}
