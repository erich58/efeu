/*	Objektlisten
	(c) 1994 Erich Fr�hst�ck
	A-1090 Wien, W�hringer Stra�e 64/6

	Version 0.4
*/

#include <EFEU/object.h>


static ALLOCTAB(list_tab, 0, sizeof(ObjList_t));


/*	Objektliste generieren
*/

ObjList_t *NewObjList(Obj_t *obj)
{
	ObjList_t *list;

	list = new_data(&list_tab);
	list->next = NULL;
	list->obj = obj;
	return list;
}


/*	Objektliste l�schen
*/

void DelObjList(ObjList_t *list)
{
	if	(list != NULL)
	{
		DelObjList(list->next);
		UnrefObj(list->obj);
		del_data(&list_tab, list);
	}
}

/*	L�nge einer Liste bestimmen
*/

int ObjListLen(const ObjList_t *list)
{
	int n;

	for (n = 0; list != NULL; n++)
		list = list->next;

	return n;
}



/*	Objektliste Referenzieren
*/

ObjList_t *RefObjList(const ObjList_t *list)
{
	ObjList_t *new_list;
	ObjList_t **ptr;

	new_list = NULL;
	ptr = &new_list;

	while (list != NULL)
	{
		*ptr = NewObjList(RefObj(list->obj));
		ptr = &(*ptr)->next;
		list = list->next;
	}

	return new_list;
}


/*	Objektlisten auswerten
*/

ObjList_t *EvalObjList(const ObjList_t *list)
{
	ObjList_t *new_list;
	ObjList_t **ptr;
	Obj_t *x;

	new_list = NULL;
	ptr = &new_list;

	while (list != NULL)
	{
		x = EvalObj(RefObj(list->obj), NULL);

		/*
		if	(x && x->lval)
		{
			*ptr = NewObjList(ConstObj(x->type, x->data));
			UnrefObj(x);
		}
		else	*ptr = NewObjList(x);
		*/

		*ptr = NewObjList(x);
		ptr = &(*ptr)->next;
		list = list->next;
	}

	return new_list;
}


/*	Objektliste zusammenstellen
*/

/* VARARGS 1 */

ObjList_t *MakeObjList(int n, ...)
{
	ObjList_t *olist;
	ObjList_t **ptr;
	va_list list;

	olist = NULL;
	ptr = &olist;
	va_start(list, n);

	while (n-- > 0)
	{
		*ptr = NewObjList(va_arg(list, Obj_t *));
		ptr = &(*ptr)->next;
	}

	va_end(list);
	return olist;
}

/*	Liste um erstes Objekt reduzieren
*/

Obj_t *ReduceObjList(ObjList_t **ptr)
{
	ObjList_t *x;
	Obj_t *obj;

	if	((x = *ptr) != NULL)
	{
		*ptr = x->next;
		obj = x->obj;
		del_data(&list_tab, x);
		return obj;
	}
	else	return NULL;
}

void ExpandObjList (ObjList_t **list, Obj_t *obj)
{
	while (*list != NULL)
		list = &(*list)->next;

	*list = NewObjList(obj);
}
