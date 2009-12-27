/*	Indizes und Vektoren
	(c) 1994 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6

	Version 0.4
*/

#include <EFEU/object.h>
#include <EFEU/stdtype.h>


Type_t Type_vec = STD_TYPE("Vec_t", Vec_t, NULL, NULL, NULL);
Vec_t Buf_vec = { NULL, NULL, 0 };


Obj_t *Vector(Vec_t *vec, size_t idx)
{
	if	(idx >= vec->dim)
	{
		errmsg(MSG_EFMAIN, 155);
		return NULL;
	}

	if	(vec->type->list && vec->type->dim && !vec->type->list->next)
	{
		Buf_vec.type = vec->type->list->type;
		Buf_vec.dim = vec->type->list->dim;
		Buf_vec.data = (char *) vec->data + idx * vec->type->size;
		return NewObj(&Type_vec, &Buf_vec);
	}

	return LvalObj(vec->type, NULL,
		(char *) vec->data + idx * vec->type->size);
}
