/*
Objekte synchronisieren

$Copyright (C) 2008 Erich Frühstück
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
#include <EFEU/Debug.h>

#if	0
static void debug_update (const char *pfx, EfiObjList *list, EfiObj *obj)
{
	fprintf(stderr, "%s(%p,%p %d)\n", pfx, list, obj, obj->refcount);
}
#else
#define	debug_update(pfx,base,obj)
#endif

void SyncLval (EfiObj *obj)
{
	if	(obj && obj->lval)
	{
		if	(obj->lval->sync)
		{
			obj->lval->sync(obj);

			if	(obj->lval->update)
				obj->lval->update(obj);
		}

		UpdateObjList(obj->list);
	}
}

void AddUpdateObj (EfiObjList **ptr, EfiObj *obj)
{
	if	(ptr && obj)
	{
		EfiObjList *list = NewObjList(obj);
		list->next = *ptr;
		*ptr = list;
		debug_update("AddUpdateObj", *ptr, obj);
	}
}

void DelUpdateObj (EfiObjList **ptr, EfiObj *obj)
{
	if	(ptr && obj)
	{
		for (; *ptr; ptr = &(*ptr)->next)
		{
			if	((*ptr)->obj == obj)
			{
				EfiObjList *list = *ptr;
				*ptr = list->next;
				list->next = NULL;
				list->obj = NULL;
				debug_update("DelUpdateObj", list, obj);
				DelObjList(list);
				return;
			}
		}
	}
}
