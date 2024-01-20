/*	Funktionen abfragen
	(c) 1994 Erich Frühstück
*/

#include <EFEU/mdmat.h>
#include <assert.h>

EfiFunc *mdfunc_add (const EfiType *type)
{
	EfiFunc *func;
	EfiObj *obj = NULL;

	for (const EfiType *t  = type; t != NULL; t = t->base)
		if ((obj = GetVar(t->vtab, "+=", NULL)))
			break;
	
	/* Check if generic function is appropriable */

	if	(!obj)
	{
		func = GetFunc((EfiType *) type, GetGlobalFunc("+"),
			2, type, 0, type, 0); 

		if	(!func)
		{
			log_note(NULL, "[mdmat:16]", "m", type2str(type));
			return NULL;
		}

		rd_deref(func);
		obj = GetVar(Type_obj.vtab, "+=", NULL);
	}

	func = GetFunc((EfiType *) type, obj2func(obj), 2, type, 1, type, 0); 
	assert(func != NULL);
	return func;
}
