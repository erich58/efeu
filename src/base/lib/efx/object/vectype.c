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

static void VecClean (const EfiType *type, void *tg);
static void VecCopy (const EfiType *type, void *tg, const void *src);
static size_t ReadVec (const EfiType *t, void *dp, IO *io);
static size_t WriteVec (const EfiType *t, const void *dp, IO *io);

static void VType2Vec (EfiFunc *func, void *rval, void **arg);
static void VType2List (EfiFunc *func, void *rval, void **arg);
static void List2VType (EfiFunc *func, void *rval, void **arg);


EfiType *VecType(EfiType *type, EfiObjList *idx)
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

EfiType *NewVecType(EfiType *type, int dim)
{
	EfiType *ntype;
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
	ntype->read = type->read ? ReadVec : NULL;
	ntype->write = type->write ? WriteVec : NULL;
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

static void VecCopy(const EfiType *type, void *tg, const void *src)
{
	const EfiType *vtype;
	size_t n;

	vtype = type->base;

	for (n = 0; n < type->size; n += vtype->size)
		vtype->copy(vtype, (char *) tg + n, (const char *) src + n);
}


/*	Löschfunktion für Vektortypen
*/

static void VecClean(const EfiType *type, void *tg)
{
	const EfiType *vtype;
	size_t n;

	vtype = type->base;

	for (n = 0; n < type->size; n += vtype->size)
		vtype->clean(vtype, (char *) tg + n);
}

static size_t ReadVec (const EfiType *t, void *dp, IO *io)
{
	return ReadVecData(t->base, t->dim, dp, io);
}

static size_t WriteVec (const EfiType *t, const void *dp, IO *io)
{
	return WriteVecData(t->base, t->dim, dp, io);
}

static void VType2Vec (EfiFunc *func, void *rval, void **arg)
{
	EfiVec *vec = rval;
	EfiType *type = func->arg[0].type;

	vec->type = type->base;
	vec->dim = type->dim;
	vec->data = arg[0];
}

static void VType2List (EfiFunc *func, void *rval, void **arg)
{
	EfiType *type = func->arg[0].type;

	Val_list(rval) = Expand_vec(type->base, arg[0], type->dim);
}

static void List2VType (EfiFunc *func, void *rval, void **arg)
{
	Assign_vec(func->type->base, rval, func->type->dim, Val_list(arg[0]));
}
