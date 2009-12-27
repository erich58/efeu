/*	Objektdaten in Liste expandieren
	(c) 1994 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6

	Version 0.4
*/

#include <EFEU/object.h>


ObjList_t *Expand_vec (Type_t *type, const void *data, size_t dim)
{
	ObjList_t *list, **ptr;

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


void Assign_vec (Type_t *type, void *data, size_t dim, const ObjList_t *list)
{
	while (list != NULL && dim != 0)
	{
		Obj2Data(RefObj(list->obj), type, data);
		data = (char *) data + type->size;
		list = list->next;
		dim--;
	}

	if	(list != NULL)
		errmsg(MSG_EFMAIN, 138);
}


void Struct2List (Func_t *func, void *rval, void **arg)
{
	ObjList_t *list, **ptr;
	Var_t *st;
	const char *p;
	Obj_t *obj;
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


void List2Struct (Func_t *func, void *rval, void **arg)
{
	ObjList_t *list;
	Var_t *st;
	char *p;
	Obj_t *obj;

	st = func->type->list;
	list = Val_list(arg[0]);
	memset(rval, 0, func->type->size);

	while (st != NULL && list != NULL)
	{
		obj = EvalObj(RefObj(list->obj),
			st->dim ? &Type_list : st->type);

		if	(obj != NULL)
		{
			p = (char *) rval + st->offset;

			if	(st->dim)
			{
				Assign_vec(st->type, p, st->dim,
					Val_list(obj->data));
			}
			else	CopyData(st->type, p, obj->data);

			UnrefObj(obj);
		}

		list = list->next;
		st = st->next;
	}
}
