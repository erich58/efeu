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


static ALLOCTAB(deftab, 0, sizeof(EfiVar));

static void data_clean (void *data)
{
	EfiVar *var = data;

	if	(var->type && var->data)
	{
		if	(var->dim)
			CleanVecData(var->type, var->dim, var->data);
		else	CleanData(var->type, var->data);
	}

	memfree(var->data);
	memfree((char *) var->name);
}

EfiVar *NewVar (EfiType *type, const char *name, size_t dim)
{
	EfiVar *var;

	var = new_data(&deftab);
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

	return var;
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

void DelVar (EfiVar *var)
{
	if	(var == NULL)		return;

	if	(var->clean)
		var->clean(var);

	del_data(&deftab, var);
}
