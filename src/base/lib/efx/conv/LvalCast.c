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

#include <EFEU/conv.h>

typedef struct {
	LVALOBJ_VAR;
} LCAST;

#define	LCAST_SIZE(type) (sizeof(LCAST) + type->size)

static EfiObj *lcast_alloc (const EfiType *type, va_list list)
{
	LCAST *lc = Lval_alloc(sizeof *lc + type->size);
	lc->base = va_arg(list, EfiObj *);
	lc->data = (lc + 1);
	AddUpdateObj(&lc->base->list, (EfiObj *) lc);
	UnrefObj(lc->base);
	return (EfiObj *) lc;
}

static void detached_free (EfiObj *obj)
{
	CleanData(obj->type, obj->data, 1);
}

static EfiLval lcast_detached = {
	"lcast_detached", NULL,
	NULL, detached_free,
	NULL, NULL, NULL,
	NULL,
};

static void lcast_free (EfiObj *obj)
{
	LCAST *lc = (LCAST *) obj;
	DelUpdateObj(&lc->base->list, (EfiObj *) lc);
	CleanData(lc->type, lc->data, 1);
}

static char *lcast_ident (const EfiObj *obj)
{
	LCAST *lc = (LCAST *) obj;
	return Obj_ident(lc->base);
}

static void lcast_update (EfiObj *obj)
{
	LCAST *lc = (LCAST *) obj;
	Obj2Data(RefObj(lc->base), lc->type, lc->data);
}

static void lcast_sync (EfiObj *obj)
{
	LCAST *lc = (LCAST *) obj;
	Obj2Data(RefObj(obj), lc->base->type, lc->base->data);
	SyncLval(lc->base);
}

static void lcast_unlink (EfiObj *obj)
{
	LCAST *lc = (LCAST *) obj;
	lc->lval = &lcast_detached;
	lc->base = NULL;
}

static EfiLval lcast_lval = {
	"lcast", NULL,
	lcast_alloc, lcast_free,
	lcast_update, lcast_sync, lcast_unlink,
	lcast_ident,
};

EfiObj *LvalCast(EfiObj *obj, EfiType *type)
{
	obj = EvalObj(obj, NULL);

	if	(obj == NULL || obj->lval == NULL)
	{
		log_note(NULL, "[efmain:167]", NULL);
		return NULL;
	}

	if	(obj->type == type)	return obj;

	return LvalObj(&lcast_lval, type, obj);
}
