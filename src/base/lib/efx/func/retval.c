/*	Rückgabewerte berechnen
	(c) 1994 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6

	Version 0.4
*/

#include <EFEU/object.h>

Obj_t *MakeRetVal(Func_t *func, void **arg)
{
	Obj_t *obj;

	if	(func->type == NULL)
	{
		obj = NULL;
		func->eval(func, &obj, arg);
	}
	else if	(func->lretval)
	{
		obj = LvalObj(func->type, func, NULL);
		func->eval(func, &obj->data, arg);
	}
	else
	{
		obj = NewObj(func->type, NULL);
		func->eval(func, obj->data, arg);
	}

	return obj;
}


void IgnoreRetVal(Func_t *func, void **arg)
{
	UnrefObj(MakeRetVal(func, arg));
}


void KonvRetVal(Type_t *type, void *data, Func_t *func, void **arg)
{
	if	(type != func->type || func->lretval)
	{
		Obj2Data(MakeRetVal(func, arg), type, data);
	}
	else	func->eval(func, data, arg);
}
