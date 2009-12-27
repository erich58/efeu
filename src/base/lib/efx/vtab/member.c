/*	Standard - Variablenabfragen
	(c) 1994 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6

	Version 0.4
*/

#include <EFEU/object.h>

Obj_t *LvalMember(const Var_t *st, const Obj_t *obj)
{
	void *data = ((EvalMember_t) st->par)(obj->data);

	if	(data == NULL)	return st->data ? ConstObj(st->type, st->data) : NULL;
	else if	(obj->lref)	return LvalObj(st->type, NULL, data);
	else			return ConstObj(st->type, data);
}


Obj_t *ConstMember(const Var_t *st, const Obj_t *obj)
{
	void *data = ((EvalMember_t) st->par)(obj->data);

	if	(data == NULL)	data = st->data;

	return data ? ConstObj(st->type, data) : NULL;
}


Obj_t *StructMember (const Var_t *st, const Obj_t *obj)
{
	void *ptr;

	if	(obj == NULL || obj->data == NULL)
		return type2Obj(st->dim ? &Type_vec : st->type);

	ptr = (char *) obj->data + st->offset;

	if	(st->dim)
	{
		Buf_vec.type = st->type;
		Buf_vec.data = ptr;
		Buf_vec.dim = st->dim;
		return NewObj(&Type_vec, &Buf_vec);
	}
	else if	(obj->lref)
	{
		return LvalObj(st->type, NULL, ptr);
	}
	else	return ConstObj(st->type, ptr);
}
