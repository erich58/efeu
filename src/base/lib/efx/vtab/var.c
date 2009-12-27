/*
Variablentabellen

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
#include <EFEU/Debug.h>

#define	TRACE	0

static ALLOCTAB(deftab, 0, sizeof(EfiVar));

static void data_clean (void *data)
{
	EfiVar *var = data;

	if	(var->type && var->data)
	{
		if	(var->dim)
			DestroyVecData(var->type, var->dim, var->data);
		else	DestroyData(var->type, var->data);
	}

	memfree(var->data);
	memfree((char *) var->name);
}

static char *var_ident (const void *data)
{
	const EfiVar *var = data;
	return mstrpaste(" ", var->type->name, var->name);
}

static void var_clean (void *data)
{
	EfiVar *var = data;

	if	(var->clean)
		var->clean(var);

#if	TRACE
	dbg_message("NewVar", DBG_TRACE, "DelVar[$2]: $1\n", NULL, "pu",
			var, (unsigned) deftab.nused);
#endif
	del_data(&deftab, var);
}

static RefType var_reftype = REFTYPE_INIT("EfiVar", var_ident, var_clean);

EfiVar *NewVar (EfiType *type, const char *name, size_t dim)
{
	EfiVar *var;

	var = new_data(&deftab);
	var->reftype = NULL;
	var->refcount = 0;
#if	TRACE
	dbg_message("NewVar", DBG_TRACE, "NewVar[$2]: $1\n", NULL, "pu",
			var, (unsigned) deftab.nused);
#endif
	memset(var, 0, sizeof(EfiVar));
	var->name = mstrcpy(name);
	var->type = type;
	var->dim = dim;

	if	(type)
	{
		dim = dim ? dim : 1;
		var->data = memalloc(type->size * dim);
		var->clean = data_clean;

		if	(type->defval)
		{
			int i;
			char *data = var->data;

			for (i = 0; i < dim; i++)
			{
				CopyData(type, data, type->defval);
				data += type->size;
			}
		}
		else	memset(var->data, 0, (size_t) type->size * dim);
	}

	return rd_init(&var_reftype, var);
}

static void ovar_clean (void *data)
{
	EfiVar *var = data;
	memfree((char *) var->name);
	memfree((char *) var->desc);
	UnrefObj(var->par);
}

static EfiObj *ovar_get (const EfiVar *var, const EfiObj *obj)
{
	return RefObj(var->par);
}

EfiVar *Obj2Var (const char *name, EfiObj *obj)
{
	EfiVar *var;

	if	(!obj)	return NULL;

	var = new_data(&deftab);
	var->name = mstrcpy(name);
	var->type = obj->type;
	var->member = ovar_get;
	var->par = obj;
	var->desc = NULL;
	var->clean = ovar_clean;
	return var;
}


EfiVar *RefVarList (const EfiVar *var)
{
	const EfiVar *p;

	for (p = var; p; p = p->next)
		rd_refer(p);

	return (EfiVar *) var;
}

void DelVarList (EfiVar *var)
{
	if	(var)
	{
		DelVarList(var->next);
		DelVar(var);
	}
}
