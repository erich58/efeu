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
#include <EFEU/stdtype.h>
#include <EFEU/printobj.h>
#include <ctype.h>

static void VecDestroy (const EfiType *type, void *tg);
static void VecClean (const EfiType *type, void *tg);
static void VecCopy (const EfiType *type, void *tg, const void *src);
static size_t ReadVec (const EfiType *t, void *dp, IO *io);
static size_t WriteVec (const EfiType *t, const void *dp, IO *io);
static int PrintVec (const EfiType *t, const void *dp, IO *io);

static void VType2Vec (EfiFunc *func, void *rval, void **arg);
static void VType2List (EfiFunc *func, void *rval, void **arg);
static void List2VType (EfiFunc *func, void *rval, void **arg);

static void V0Type2Vec (EfiFunc *func, void *rval, void **arg);
static void V0Type2List (EfiFunc *func, void *rval, void **arg);
static void List2V0Type (EfiFunc *func, void *rval, void **arg);

static void V0Destroy (const EfiType *type, void *tg);
static void V0Clean (const EfiType *type, void *tg);
static void V0Copy (const EfiType *type, void *tg, const void *src);

EfiType *VecType (EfiType *type, EfiObjList *idx)
{
	return idx ? NewVecType(VecType(type, idx->next),
		Obj2int(RefObj(idx->obj))) : type;
}


/*	Vektortype generieren
*/

EfiType *NewVecType (EfiType *type, int dim)
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

	if	(dim)
	{
		ntype = NewType(name);
		ntype->dim = dim;
		ntype->base = type;
		ntype->destroy = type->destroy ? VecDestroy : NULL;
		ntype->clean = type->clean ? VecClean : NULL;
		ntype->copy = type->copy ? VecCopy : NULL;
		ntype->size = dim * type->size;
		ntype->recl = dim * type->recl;
		ntype->read = type->read ? ReadVec : NULL;
		ntype->write = type->write ? WriteVec : NULL;
		ntype->print = PrintVec;
		AddType(ntype);
		p = msprintf("%s & ()", ntype->name);
		SetFunc(0, &Type_vec, p, VType2Vec);
		SetFunc(FUNC_RESTRICTED, &Type_list, p, VType2List);
		memfree(p);
		p = msprintf("%s (List_t)", ntype->name);
		SetFunc(0, ntype, p, List2VType);
		memfree(p);
	}
	else
	{
		ntype = NewType(name);
		ntype->dim = dim;
		ntype->base = &Type_vec;
		ntype->destroy = V0Destroy;
		ntype->clean = V0Clean;
		ntype->copy = V0Copy;
		ntype->size = sizeof(EfiVec *);
		ntype->recl = 0;
		ntype->read = Read_vec;
		ntype->write = Write_vec;
		ntype->print = Print_vec;
		ntype->defval = memalloc(sizeof(EfiVec *));
		Val_ptr(ntype->defval) = NewEfiVec(type, NULL, 0);
		AddType(ntype);
		p = msprintf("%s & ()", ntype->name);
		SetFunc(0, &Type_vec, p, V0Type2Vec);
		SetFunc(FUNC_RESTRICTED, &Type_list, p, V0Type2List);
		memfree(p);
		p = msprintf("%s (List_t)", ntype->name);
		SetFunc(0, ntype, p, List2V0Type);
		memfree(p);
	}

	return ntype;
}


/*	Kopierfunktion für Vektortypen
*/

static void VecCopy (const EfiType *type, void *tg, const void *src)
{
	const EfiType *vtype;
	size_t n;

	vtype = type->base;

	for (n = 0; n < type->size; n += vtype->size)
		vtype->copy(vtype, (char *) tg + n, (const char *) src + n);
}

static void V0Destroy (const EfiType *type, void *tg)
{
	rd_deref(Val_ptr(tg));
}

static void V0Clean (const EfiType *type, void *tg)
{
	if	(Val_ptr(tg))
		EfiVec_resize(Val_ptr(tg), 0);
}


static void V0Copy (const EfiType *type, void *tg, const void *src)
{
	Val_ptr(tg) = EfiVec_copy(Val_ptr(src));
}


/*	Löschfunktion für Vektortypen
*/

static void VecDestroy (const EfiType *type, void *tg)
{
	const EfiType *vtype;
	size_t n;

	vtype = type->base;

	for (n = 0; n < type->size; n += vtype->size)
		vtype->destroy(vtype, (char *) tg + n);
}

static void VecClean (const EfiType *type, void *tg)
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

static int PrintVec (const EfiType *t, const void *dp, IO *io)
{
	return PrintVecData(io, t->base, dp, t->dim);
}

static void VType2Vec (EfiFunc *func, void *rval, void **arg)
{
	EfiType *type = func->arg[0].type;
	Val_ptr(rval) = NewEfiVec(type->base, arg[0], type->dim);
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

static void V0Type2Vec (EfiFunc *func, void *rval, void **arg)
{
	Val_ptr(rval) = rd_refer(Val_ptr(arg[0]));
}

static void V0Type2List (EfiFunc *func, void *rval, void **arg)
{
	EfiVec *vec = Val_ptr(arg[0]);
	Val_list(rval) = vec ? Expand_vec(vec->type, vec->buf.data,
		vec->buf.used) : NULL;
}

static void List2V0Type (EfiFunc *func, void *rval, void **arg)
{
	EfiObjList *list;
	EfiVec *vec;
	unsigned n;

	vec = Val_ptr(func->type->defval);

	if	(vec)
	{
		vec = NewEfiVec(vec->type, NULL, 0);
		list = Val_list(arg[0]);
		
		for (n = 0; list != NULL; list = list->next)
			UnrefEval(AssignObj(Vector(vec, n++),
				EvalObj(RefObj(list->obj), NULL)));
	}

	Val_ptr(rval) = vec;
}
