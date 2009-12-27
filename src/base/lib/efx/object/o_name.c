/*
Namensobjekt

$Copyright (C) 1994 Erich Fr�hst�ck
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
#include <EFEU/stdtype.h>


/*	Kopier und Auswertungsfunktionen
*/

static void n_clean (const Type_t *type, Name_t *tg);
static void n_copy (const Type_t *type, Name_t *tg, const Name_t *src);
static Obj_t *n_meval (const Type_t *type, const Name_t *ptr);
static Obj_t *n_seval (const Type_t *type, const Name_t *ptr);

Type_t Type_mname = EVAL_TYPE("_MemberName_", Name_t,
	(Eval_t) n_meval, (Clean_t) n_clean, (Copy_t) n_copy);
Type_t Type_sname = EVAL_TYPE("_ScopeName_", Name_t,
	(Eval_t) n_seval, (Clean_t) n_clean, (Copy_t) n_copy);

Name_t Buf_name = { NULL, NULL };

static void n_clean(const Type_t *type, Name_t *tg)
{
	memfree(tg->name);
	UnrefObj(tg->obj);
	tg->name = NULL;
	tg->obj = NULL;
}


static void n_copy(const Type_t *type, Name_t *tg, const Name_t *src)
{
	tg->name = mstrcpy(src->name);
	tg->obj = RefObj(src->obj);
}


/*	Namen auswerten
*/

static Obj_t *n_meval(const Type_t *type, const Name_t *name)
{
	return GetMember(RefObj(name->obj), name->name);
}

static Obj_t *n_seval(const Type_t *type, const Name_t *name)
{
	return GetScope(RefObj(name->obj), name->name);
}

Obj_t *Var2Obj (Var_t *var, const Obj_t *obj)
{
	if	(var == NULL)
		return NULL;

	if	(var->member)
		return var->member(var, obj);

	if	(!var->data)
		return ConstObj(var->type, NULL);

	if	(var->dim)
	{
		Buf_vec.type = var->type;
		Buf_vec.data = var->data;
		Buf_vec.dim = var->dim;
		return NewObj(&Type_vec, &Buf_vec);
	}

	if	(var->type->dim)
	{
		Buf_vec.type = var->type->base;
		Buf_vec.dim = var->type->dim;
		Buf_vec.data = var->data;
		return NewObj(&Type_vec, &Buf_vec);
	}

	return LvalObj(&Lval_ptr, var->type, var->data);
}


Obj_t *GetMember (Obj_t *obj, const char *name)
{
	Type_t *type;
	Obj_t *x;

	obj = EvalObj(obj, NULL);

	if	(obj == NULL || obj->type == NULL || name == NULL)
		return NULL;

	for (type = obj->type; type != NULL; type = type->base)
	{
		if	((x = GetVar(type->vtab, name, obj)) != NULL)
		{
			UnrefObj(obj);
			return x;
		}

		if	(type->dim)	break;
	}

	reg_cpy(1, obj->type->name);
	reg_cpy(2, name);
	errmsg(MSG_EFMAIN, 164);
	return NULL;
}


Obj_t *GetScope (Obj_t *obj, const char *name)
{
	VarTab_t *tab = Obj2Ptr(obj, &Type_vtab);

	if	(tab)
	{
		obj = GetVar(tab, name, NULL);
		DelVarTab(tab);

		if	(obj)	return obj;
	}

	message(NULL, MSG_EFMAIN, 166, 1, name);
	return NULL;
}
