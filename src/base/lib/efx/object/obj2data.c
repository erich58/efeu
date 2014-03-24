/*
Objekte konvertieren

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

static void swap_data (size_t size, char *a, char *b)
{
	int i;

	for (i = 0; i < size; i++)
	{
		register char c = a[i];
		a[i] = b[i];
		b[i] = c;
	}
}


EfiObj *AssignTerm (const char *name, EfiObj *left, EfiObj *right)
{
	void *func;
	EfiObjList *list;

	if	(left == NULL)
	{
		log_note(NULL, "[efmain:211]", NULL);
		rd_deref(right);
		return NULL;
	}
	else if	((func = GetTypeFunc(left->type, name)) != NULL)
	{
		list = NewObjList(left);
		list->next = NewObjList(right);
		left = EvalFunc(func, list);
		DelObjList(list);
		return left;
	}
	else
	{
		log_note(NULL, left->lval ? "[efmain:215]" : "[efmain:213]",
			"ss", name, left->type->name);
		rd_deref(right);
		rd_deref(left);
		return NULL;
	}
}


EfiObj *AssignObj (EfiObj *left, EfiObj *right)
{
	if	(left == NULL || left->lval == NULL)
		return AssignTerm("=", left, right);

	right = EvalObj(right, left->type);

	if	(right == NULL)
	{
		log_note(NULL, "[efmain:212]", NULL);
		return left;
	}

#if	LVAL_ASSIGN
	if	(left->lval->assign)
	{
		left->lval->assign(left, right->data);
	}
	else
#endif
	if	(left->data == right->data)
	{
		;
	}
	else if	(right->lval == NULL && right->refcount == 1)
	{
		swap_data(left->type->size, left->data, right->data);
	}
	else	AssignData(left->type, left->data, right->data);

	rd_deref(right);
	SyncLval(left);
	return left;
}


void Obj2Data (EfiObj *obj, EfiType *type, void *ptr)
{
	if	(type == NULL)
	{
		rd_deref(obj);
		return;
	}

	obj = EvalObj(obj, type);

	if	(obj == NULL)
	{
		memset(ptr, 0, type->size);
	}
	else if	(obj->lval == NULL && obj->refcount == 1)
	{
		memcpy(ptr, obj->data, type->size);
		obj->data = NULL;
		rd_deref(obj);
	}
	else
	{
		if	(ptr != obj->data)
			CopyData(type, ptr, obj->data);

		rd_deref(obj);
	}
}

void Obj2VecData (EfiObj *obj, EfiType *type, size_t dim, void *ptr)
{
	EfiObjList *list;

	if	(type == NULL || dim == 0 || ptr == NULL)
	{
		UnrefObj(obj);
		return;
	}

	obj = EvalObj(obj, &Type_list);
	list = obj ? Val_list(obj->data) : NULL;

	while (list && dim)
	{
		Obj2Data(RefObj(list->obj), type, ptr);
		ptr = (char *) ptr + type->size;
		list = list->next;
		dim--;
	}

	CleanVecData(type, dim, ptr, 0);
	rd_deref(obj);
}


void *Obj2Ptr (EfiObj *obj, EfiType *type)
{
	void *ptr = NULL;
	Obj2Data(obj, type, &ptr);
	return ptr;
}
