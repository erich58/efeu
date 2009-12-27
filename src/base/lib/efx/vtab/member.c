/*
Standard - Variablenabfragen

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

Obj_t *LvalMember(const Var_t *st, const Obj_t *obj)
{
	void *data = obj ? ((EvalMember_t) st->par)(obj->data) : NULL;

	if	(data == NULL)	return ConstObj(st->type, st->data);
	else if	(obj->lval)	return LvalObj(&Lval_ptr, st->type, data);
	else			return ConstObj(st->type, data);
}


Obj_t *ConstMember(const Var_t *st, const Obj_t *obj)
{
	void *data = obj ? ((EvalMember_t) st->par)(obj->data) : NULL;

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
	else if	(obj->lval)
	{
		return LvalObj(&Lval_ptr, st->type, ptr);
	}
	else	return ConstObj(st->type, ptr);
}
