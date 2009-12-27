/*
Objekt konvertieren

$Copyright (C) 1997 Erich Frühstück
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
