/*
Namensobjekt

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
#include <EFEU/stdtype.h>


/*	Kopier und Auswertungsfunktionen
*/

static void n_clean (const EfiType *type, void *data, int mode);
static void n_copy (const EfiType *type, void *tg, const void *src);
static EfiObj *n_meval (const EfiType *type, const void *ptr);
static EfiObj *n_seval (const EfiType *type, const void *ptr);

EfiType Type_mname = EVAL_TYPE("_MemberName_", EfiName, n_meval,
	NULL, n_clean, n_copy);
EfiType Type_sname = EVAL_TYPE("_ScopeName_", EfiName, n_seval,
	NULL, n_clean, n_copy);

EfiName Buf_name = { NULL, NULL };

static void n_clean(const EfiType *type, void *data, int mode)
{
	EfiName *tg = data;
	memfree(tg->name);
	UnrefObj(tg->obj);
	tg->name = NULL;
	tg->obj = NULL;
}


static void n_copy(const EfiType *type, void *tptr, const void *sptr)
{
	EfiName *tg = tptr;
	const EfiName *src = sptr;
	tg->name = mstrcpy(src->name);
	tg->obj = RefObj(src->obj);
}


/*	Namen auswerten
*/

static EfiObj *n_meval(const EfiType *type, const void *ptr)
{
	const EfiName *name = ptr;
	return GetMember(RefObj(name->obj), name->name);
}

static EfiObj *n_seval(const EfiType *type, const void *ptr)
{
	const EfiName *name = ptr;
	return GetScope(RefObj(name->obj), name->name);
}

EfiObj *Var2Obj (EfiStruct *var, const EfiObj *obj)
{
	void *ptr;

	if	(var == NULL || obj == NULL)
		return NULL;

	if	(var->member)
		return var->member(var, obj);

	ptr = (char *) obj->data + var->offset;

	if	(var->dim)
		return EfiVecObj(var->type, ptr, var->dim);

	if	(var->type->dim)
		return EfiVecObj(var->type->base, ptr, var->type->dim);

	if	(obj->lval)
		return LvalObj(&Lval_ptr, var->type, ptr);

	return ConstObj(var->type, ptr);
}


EfiObj *GetMember (EfiObj *obj, const char *name)
{
	EfiType *type;
	EfiObj *x;

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

	log_note(NULL, "[efmain:164]", "ss", obj->type->name, name);
	return NULL;
}


EfiObj *GetScope (EfiObj *obj, const char *name)
{
	EfiVarTab *tab = Obj2Ptr(obj, &Type_vtab);

	if	(tab)
	{
		obj = GetVar(tab, name, NULL);
		DelVarTab(tab);

		if	(obj)	return obj;
	}

	log_note(NULL, "[efmain:166]", "s", name);
	return NULL;
}
