/*
Vektortypen

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
#include <ctype.h>

static void VecClean (const Type_t *type, void *tg);
static void VecCopy (const Type_t *type, void *tg, const void *src);
static size_t IOVec (const Type_t *t, iofunc_t f, void *p, void *d, size_t n);

static void VType2Vec (Func_t *func, void *rval, void **arg);
static void VType2List (Func_t *func, void *rval, void **arg);
static void List2VType (Func_t *func, void *rval, void **arg);


Type_t *VecType(Type_t *type, ObjList_t *idx)
{
	if	(idx != NULL)
	{
		type = NewVecType(VecType(type, idx->next),
			Obj2long(RefObj(idx->obj)));
	}

	return type;
}


/*	Vektortype generieren
*/

Type_t *NewVecType(Type_t *type, int dim)
{
	Type_t *ntype;
	char *name;
	char *p;

	name = msprintf("_%s%d", type->name, dim);

	if	((ntype = GetType(name)) != NULL)
	{
		memfree(name);
		return ntype;
	}

	ntype = NewType(name);
	ntype->dim = dim;
	ntype->base = type;
	ntype->clean = type->clean ? VecClean : NULL;
	ntype->copy = type->copy ? VecCopy : NULL;
	ntype->size = dim * type->size;
	ntype->recl = dim * type->recl;
	ntype->iodata = type->iodata ? IOVec : NULL;
	AddType(ntype);
	p = msprintf("%s & ()", ntype->name);
	SetFunc(0, &Type_vec, p, VType2Vec);
	SetFunc(FUNC_RESTRICTED, &Type_list, p, VType2List);
	memfree(p);
	p = msprintf("%s (List_t)", ntype->name);
	SetFunc(0, ntype, p, List2VType);
	memfree(p);
	return ntype;
}


/*	Kopierfunktion für Vektortypen
*/

static void VecCopy(const Type_t *type, void *tg, const void *src)
{
	const Type_t *vtype;
	ulong_t n;

	vtype = type->base;

	for (n = 0; n < type->size; n += vtype->size)
		vtype->copy(vtype, (char *) tg + n, (const char *) src + n);
}


/*	Löschfunktion für Vektortypen
*/

static void VecClean(const Type_t *type, void *tg)
{
	const Type_t *vtype;
	ulong_t n;

	vtype = type->base;

	for (n = 0; n < type->size; n += vtype->size)
		vtype->clean(vtype, (char *) tg + n);
}


static size_t IOVec (const Type_t *t, iofunc_t f, void *p, void *dp, size_t n)
{
	Type_t *base = t->base;
	return base->iodata(base, f, p, dp, n * (t->size / base->size));
}

static void VType2Vec (Func_t *func, void *rval, void **arg)
{
	Vec_t *vec = rval;
	Type_t *type = func->arg[0].type;

	vec->type = type->base;
	vec->dim = type->size / type->base->size;
	vec->data = arg[0];
}

static void VType2List (Func_t *func, void *rval, void **arg)
{
	Type_t *type = func->arg[0].type;

	Val_list(rval) = Expand_vec(type->base, arg[0],
		type->size / type->base->size);
}

static void List2VType (Func_t *func, void *rval, void **arg)
{
	Type_t *base = func->type->base;
	Assign_vec(base, rval, func->type->size / base->size, Val_list(arg[0]));
}
