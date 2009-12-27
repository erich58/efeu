/*	Objekte konvertieren
	(c) 1994 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6

	Version 0.4
*/

#include <EFEU/object.h>

static void swap_data(size_t size, char *a, char *b)
{
	int i;

	for (i = 0; i < size; i++)
	{
		register char c = a[i];
		a[i] = b[i];
		b[i] = c;
	}
}


Obj_t *AssignTerm(const char *name, Obj_t *left, Obj_t *right)
{
	void *func;
	ObjList_t *list;

	if	(left == NULL)
	{
		errmsg(MSG_EFMAIN, 211);
		UnrefObj(right);
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
		reg_cpy(1, name);
		reg_cpy(2, left->type->name);
		errmsg(MSG_EFMAIN, left->lref ? 215 : 213);
		UnrefObj(right);
		UnrefObj(left);
		return NULL;
	}
}


Obj_t *AssignObj(Obj_t *left, Obj_t *right)
{
	if	(left == NULL || left->lref == NULL)
		return AssignTerm("=", left, right);

	right = EvalObj(right, left->type);

	if	(right == NULL)
	{
		errmsg(MSG_EFMAIN, 212);
	}
	else if	(right->lref == NULL && right->refcount == 1)
	{
		swap_data(left->type->size, left->data, right->data);
	}
	else	AssignData(left->type, left->data, right->data);

	UnrefObj(right);
	return left;
}


void Obj2Data(Obj_t *obj, Type_t *type, void *ptr)
{
	if	(type == NULL)
	{
		UnrefObj(obj);
		return;
	}

	obj = EvalObj(obj, type);

	if	(obj == NULL)
	{
		memset(ptr, 0, type->size);
	}
	else if	(obj->lref == NULL && obj->refcount == 1)
	{
		memcpy(ptr, obj->data, type->size);
		DeleteObj(obj);
	}
	else
	{
		if	(ptr != obj->data)
			CopyData(type, ptr, obj->data);

		UnrefObj(obj);
	}
}


void *Obj2Ptr(Obj_t *obj, Type_t *type)
{
	void *ptr = NULL;
	Obj2Data(obj, type, &ptr);
	return ptr;
}
