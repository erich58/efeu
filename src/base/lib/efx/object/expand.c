/*
Objektdaten in Liste expandieren

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


EfiObjList *Expand_vec (EfiType *type, const void *data, size_t dim)
{
	EfiObjList *list, **ptr;

	list = NULL;
	ptr = &list;

	while (dim-- != 0)
	{
		*ptr = NewObjList(ConstObj(type, data));
		ptr = &(*ptr)->next;
		data = (const char *) data + type->size;
	}

	return list;
}


void Assign_vec (EfiType *type, void *data, size_t dim, const EfiObjList *list)
{
	while (list != NULL && dim != 0)
	{
		Obj2Data(RefObj(list->obj), type, data);
		data = (char *) data + type->size;
		list = list->next;
		dim--;
	}

	if	(list != NULL)
		dbg_note(NULL, "[efmain:138]", NULL);
}


void Struct2List (EfiFunc *func, void *rval, void **arg)
{
	EfiObjList *list, **ptr;
	EfiVar *st;
	const char *p;
	EfiObj *obj;
	char *data;

	list = NULL;
	ptr = &list;
	st = func->arg[0].type->list;
	data = arg[0];

	while (st != NULL)
	{
		p = data + st->offset;

		if	(st->dim)
		{
			obj = Obj_list(Expand_vec(st->type, p, st->dim));
		}
		else	obj = ConstObj(st->type, p);

		*ptr = NewObjList(obj);
		ptr = &(*ptr)->next;
		st = st->next;
	}

	Val_list(rval) = list;
}


void List2Struct (EfiFunc *func, void *rval, void **arg)
{
	EfiObjList *list;
	EfiVar *st;
	char *p, *s;
	EfiObj *obj;

	list = Val_list(arg[0]);
	memset(rval, 0, func->type->size);

	for (st = func->type->list; st != NULL; st = st->next)
	{
		p = (char *) rval + st->offset;

		if	(list)
		{
			obj = EvalObj(RefObj(list->obj),
				st->dim ? &Type_list : st->type);
			list = list->next;
		}
		else	obj = NULL;

		if	(obj)
		{
			if	(st->dim)
			{
				Assign_vec(st->type, p, st->dim,
					Val_list(obj->data));
			}
			else	CopyData(st->type, p, obj->data);

			UnrefObj(obj);
		}
		else
		{
			if	(func->type->defval)
			{
				s = (char *) func->type->defval + st->offset;
			}
			else	s = st->data;

			if	(st->dim)
			{
				CopyVecData(st->type, st->dim, p, s);
			}
			else	CopyData(st->type, p, s);
		}
	}
}
