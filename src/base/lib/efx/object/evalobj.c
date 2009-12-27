/*	Objekte auswerten/konvertieren
	(c) 1994 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6

	Version 0.4
*/

#include <EFEU/object.h>
#include <EFEU/konvobj.h>


void UnrefEval(Obj_t *obj)
{
	UnrefObj(EvalObj(obj, NULL));
}


Obj_t *EvalObj(Obj_t *x, Type_t *type)
{
	Obj_t *y;

	while (x && x->type)
	{
		y = x;

		if	(y->type == type)
		{
			break;
		}
		else if	(y->type->eval)
		{
			x = y->type->eval(y->type, y->data);

			if	(x == y)
			{
				reg_cpy(1, x->type->name);
				errmsg(MSG_EFMAIN, 137);
				UnrefObj(x);
				x = NULL;
			}
		}
		/*
		else if	(type && !type->eval)
		*/
		else if	(type)
		{
			x = KonvObj(y, type);
			UnrefObj(y);
			break;
		}
		else	break;

		UnrefObj(y);
	}

	return x;
}
