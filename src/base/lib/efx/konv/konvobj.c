/*	Objekt konvertieren
	(c) 1997 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6

	Version 0.4
*/

#include <EFEU/object.h>
#include <EFEU/konvobj.h>


Obj_t *KonvObj(const Obj_t *obj, Type_t *def)
{
	Konv_t konv;

	if	(obj == NULL)
		return NULL;
	
	if	(obj->type == def || def == NULL)
		return RefObj(obj);

	if	(GetKonv(&konv, obj->type, def))
	{
		Obj_t *x = NewObj(def, NULL);
		KonvData(&konv, x->data, obj->data);
		return x;
	}

/*	Konvertierung nicht möglich
*/
	if	(obj->type == NULL)
		reg_set(1, NULL);
	else if	(obj->type == &Type_undef)
		reg_set(1, msprintf("(%s) %s", obj->type->name,
			Val_str(obj->data)));
	else	reg_cpy(1, obj->type->name);

	reg_cpy(2, def ? def->name : NULL);
	errmsg(MSG_EFMAIN, 161);
	return NULL;
}
