/*
Stringpool Eintrag

$Copyright (C) 2007 Erich Frühstück
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
#include <EFEU/StrPool.h>

typedef struct {
	OBJECT_VAR;
	EfiObj *base;
	StrPool *pool;
	size_t *idx;
} StrPoolPar;

static EfiObj *pool_alloc (const EfiType *type, va_list list)
{
	StrPoolPar *par = (void *) Obj_alloc(sizeof *par + Type_str.size);
	par->data = (par + 1);
	par->base = RefObj(va_arg(list, EfiObj *));
	par->pool = rd_refer(va_arg(list, StrPool *));
	par->idx = va_arg(list, size_t *);
	return (void *) par;
}

static void pool_free (EfiObj *obj)
{
	StrPoolPar *par = (void *) obj;
	UnrefObj(par->base);
	rd_deref(par->pool);
	CleanData(&Type_str, par->data, 1);
}

static void pool_update (EfiObj *obj)
{
	StrPoolPar *par = (void *) obj;
	char *p = StrPool_get(par->pool, *par->idx);

	if	(mstrcmp(Val_str(par->data), p) != 0)
	{
		memfree(Val_str(par->data));
		Val_str(par->data) = mstrcpy(p);
	}
}

static void pool_sync (EfiObj *obj)
{
	StrPoolPar *par = (void *) obj;
	char *p = StrPool_get(par->pool, *par->idx);

	if	(mstrcmp(Val_str(par->data), p) != 0)
		*par->idx = StrPool_add(par->pool, Val_str(par->data));
}

static char *pool_ident (const EfiObj *obj)
{
	StrPoolPar *par = (void *) obj;
	char *p = rd_ident(par->pool);
	char *id = msprintf("%s: %u", p, (unsigned) *par->idx);
	memfree(p);
	return id;
}

static EfiLval Lval_pool = {
	"StrPoolObj", "String pool entry",
	pool_alloc,
	pool_free,
	pool_update,
	pool_sync,
	NULL,
	pool_ident,
};


EfiObj *StrPoolObj (const EfiObj *base, const StrPool *pool, size_t *idx)
{
	return LvalObj(&Lval_pool, &Type_str, base, pool, idx);
}
