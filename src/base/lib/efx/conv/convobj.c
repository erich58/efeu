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
#include <EFEU/conv.h>


EfiObj *KonvObj (const EfiObj *obj, EfiType *def)
{
	EfiKonv konv;
	char *p;

	if	(obj == NULL)
		return NULL;
	
	if	(obj->type == def || def == NULL)
		return RefObj(obj);

	if	(def == &Type_obj)
		return obj2Obj(RefObj(obj));

	if	(obj->type == &Type_obj)
		return KonvObj(Val_obj(obj->data), def);

	if	(GetKonv(&konv, obj->type, def))
	{
		EfiObj *x = NewObj(def, NULL);
		KonvData(&konv, x->data, obj->data);
		return x;
	}

/*	Konvertierung nicht möglich
*/
	if	(obj->type == NULL)
		p = NULL;
	else if	(obj->type == &Type_undef)
		p = msprintf("(%s) %s", obj->type->name, Val_str(obj->data));
	else	p = mstrcpy(obj->type->name);

	log_note(NULL, "[efmain:161]", "ms", p, def ? def->name : NULL);
	return NULL;
}
