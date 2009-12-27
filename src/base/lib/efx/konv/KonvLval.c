/*
Konvertieren von L-Werten

$Copyright (C) 2001 Erich Frühstück
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

#include <EFEU/konvobj.h>

typedef struct {
	OBJECT_VAR;
	Obj_t *base;
} LCAST;


#define	LCAST_SIZE(type) (sizeof(LCAST) + type->size)

static Obj_t *lcast_alloc (Type_t *type, va_list list)
{
	LCAST *lc = (LCAST *) Obj_alloc(LCAST_SIZE(type));
	lc->base = va_arg(list, Obj_t *);
	lc->data = (void *) (lc + 1);
	return (Obj_t *) lc;
}

static void lcast_free (Obj_t *obj)
{
	LCAST *lc = (LCAST *) obj;
	CleanData(lc->type, lc->data);
	UnrefObj(lc->base);
	Obj_free(obj, LCAST_SIZE(lc->type));
}

static char *lcast_ident (Obj_t *obj)
{
	LCAST *lc = (LCAST *) obj;
	return mstrcpy(lc->base->type->name);
}

static void lcast_update (Obj_t *obj)
{
	LCAST *lc = (LCAST *) obj;
	Obj2Data(RefObj(lc->base), lc->type, lc->data);
}

static void lcast_sync (Obj_t *obj)
{
	LCAST *lc = (LCAST *) obj;
	Obj2Data(RefObj(obj), lc->base->type, lc->base->data);
}

static Lval_t lcast_lval = {
	lcast_alloc, lcast_free,
	lcast_update, lcast_sync,
	lcast_ident,
};


Obj_t *KonvLval(Obj_t *obj, Type_t *type)
{
	obj = EvalObj(obj, NULL);

	if	(obj == NULL || obj->lval == NULL)
	{
		errmsg(MSG_EFMAIN, 167);
		return NULL;
	}

	if	(obj->type == type)	return obj;

	return LvalObj(&lcast_lval, type, obj);
}
