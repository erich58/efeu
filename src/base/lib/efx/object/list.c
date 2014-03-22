/*
Objektlisten

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


static ALLOCTAB(list_tab, "EfiObjList", 0, sizeof(EfiObjList));


/*	Objektliste generieren
*/

EfiObjList *NewObjList(EfiObj *obj)
{
	EfiObjList *list;

	list = new_data(&list_tab);
	list->next = NULL;
	list->obj = obj;
	return list;
}


/*	Objektliste löschen
*/

void DelObjList(EfiObjList *list)
{
	if	(list != NULL)
	{
		DelObjList(list->next);
		UnrefObj(list->obj);
		del_data(&list_tab, list);
	}
}


/*	Updateliste löschen
*/

void UnlinkUpdateList (EfiObjList *list)
{
	if	(!list)	return;

	while (list != NULL)
	{
		void *data = list;
		EfiObj *obj = list->obj;
		list = list->next;

		io_puts("unlink(", ioerr);
		Obj_putkey(obj, ioerr);
		io_puts(")\n", ioerr);

		if	(obj->lval && obj->lval->unlink)
			obj->lval->unlink(obj);

		del_data(&list_tab, data);
	}
}

/*	Länge einer Liste bestimmen
*/

int ObjListLen(const EfiObjList *list)
{
	int n;

	for (n = 0; list != NULL; n++)
		list = list->next;

	return n;
}



/*	Objektliste Referenzieren
*/

EfiObjList *RefObjList(const EfiObjList *list)
{
	EfiObjList *new_list;
	EfiObjList **ptr;

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

EfiObjList *EvalObjList(const EfiObjList *list)
{
	EfiObjList *new_list;
	EfiObjList **ptr;
	EfiObj *x;

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

EfiObjList *MakeObjList(int n, ...)
{
	EfiObjList *olist;
	EfiObjList **ptr;
	va_list list;

	olist = NULL;
	ptr = &olist;
	va_start(list, n);

	while (n-- > 0)
	{
		*ptr = NewObjList(va_arg(list, EfiObj *));
		ptr = &(*ptr)->next;
	}

	va_end(list);
	return olist;
}

/*	Liste um erstes Objekt reduzieren
*/

EfiObj *ReduceObjList(EfiObjList **ptr)
{
	EfiObjList *x;
	EfiObj *obj;

	if	((x = *ptr) != NULL)
	{
		*ptr = x->next;
		obj = x->obj;
		del_data(&list_tab, x);
		return obj;
	}
	else	return NULL;
}

void ExpandObjList (EfiObjList **list, EfiObj *obj)
{
	while (*list != NULL)
		list = &(*list)->next;

	*list = NewObjList(obj);
}
