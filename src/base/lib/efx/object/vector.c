/*
Indizes und Vektoren

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

	return LvalObj(&Lval_ptr, vec->type,
		(char *) vec->data + idx * vec->type->size);
}
