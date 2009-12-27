/*	Variablentabellen
	(c) 1994 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6

	Version 0.4
*/

#include <EFEU/object.h>


static ALLOCTAB(deftab, 0, sizeof(Var_t));


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
	else	var->data = NULL;

	return var;
}


void DelVar (Var_t *var)
{
	if	(var == NULL)		return;

	if	(del_data(&deftab, var))
	{
		if	(var->type && var->data)
		{
			if	(var->dim)
				CleanVecData(var->type, var->dim, var->data);
			else	CleanData(var->type, var->data);
		}

		memfree((char *) var->name);
		memfree(var->data);
	}
}
