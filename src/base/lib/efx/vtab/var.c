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


static ALLOCTAB(deftab, 0, sizeof(Var_t));

static void data_clean (Var_t *var)
{
	if	(var->type && var->data)
	{
		if	(var->dim)
			CleanVecData(var->type, var->dim, var->data);
		else	CleanData(var->type, var->data);
	}

	memfree(var->data);
	memfree((char *) var->name);
}

Var_t *NewVar (Type_t *type, const char *name, size_t dim)
{
	Var_t *var;

	var = new_data(&deftab);
	memset(var, 0, sizeof(Var_t));
	var->name = mstrcpy(name);
	var->type = type;
	var->dim = dim;

	if	(type)
	{
		dim = dim ? dim : 1;
		var->data = memalloc(type->size * dim);
		var->clean = (clean_t) data_clean;

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


void DelVar (Var_t *var)
{
	if	(var == NULL)		return;

	if	(var->clean)
		var->clean(var);

	del_data(&deftab, var);
}
