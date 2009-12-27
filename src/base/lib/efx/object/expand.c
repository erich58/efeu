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
	EfiStruct *st;
	EfiObj *base;
	char *data;

	list = NULL;
	ptr = &list;
	st = func->arg[0].type->list;
	data = arg[0];

	if	(func->arg[0].lval)
		base = LvalObj(&Lval_ptr, func->arg[0].type, arg[0]);
	else	base = ConstObj(func->arg[0].type, arg[0]);

	while (st != NULL)
	{
		*ptr = NewObjList(Var2Obj(st, base));
		ptr = &(*ptr)->next;
		st = st->next;
	}

	Val_list(rval) = list;
	UnrefObj(base);
}


void List2Struct (EfiFunc *func, void *rval, void **arg)
{
	EfiObjList *list;
	EfiObj *base, *tg;
	EfiStruct *st;

	list = Val_list(arg[0]);
	base = LvalObj(&Lval_ptr, func->type, rval);

	for (st = func->type->list; st != NULL; st = st->next)
	{
		tg = Var2Obj(st, base);

		if	(list)
		{
			AssignObj(tg, RefObj(list->obj));
			list = list->next;
		}
		else	CleanData(tg->type, tg->data, 0);
	}

	UnrefObj(base);
}
