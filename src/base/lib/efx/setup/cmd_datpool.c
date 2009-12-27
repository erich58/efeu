/*
Datenpool

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

#include <EFEU/ftools.h>
#include <EFEU/object.h>
#include <EFEU/cmdconfig.h>
#include <EFEU/printobj.h>
#include <EFEU/locale.h>
#include <EFEU/preproc.h>
#include <EFEU/pconfig.h>
#include <EFEU/DatPool.h>

EfiType Type_DatPool = REF_TYPE("DatPool", DatPool *);

#define	POOL(x)	((DatPool **) (x))[0]

static void f_null (EfiFunc *func, void *rval, void **arg)
{
	POOL(rval) = NULL;
}

static void f_alloc (EfiFunc *func, void *rval, void **arg)
{
	POOL(rval) = DatPool_dyn(Val_int(arg[0]));
}

static void f_load (EfiFunc *func, void *rval, void **arg)
{
	POOL(rval) = DatPool_map(Val_str(arg[0]));
}

static void f_add (EfiFunc *func, void *rval, void **arg)
{
	DatPool *pool = Val_ptr(arg[0]);
	EfiObj *obj = arg[1];
	Val_uint(rval) = DatPool_add(pool, obj->data, obj->type->size);
}

static void f_get (EfiFunc *func, void *rval, void **arg)
{
	DatPool *pool = Val_ptr(arg[0]);
	EfiType *type = Val_type(arg[2]);
	Val_obj(rval) = ConstObj(type, DatPool_get(pool, Val_int(arg[1])));
}

static void f_save (EfiFunc *func, void *rval, void **arg)
{
	DatPool *pool = Val_ptr(arg[0]);
	DatPool_create(pool, Val_str(arg[1]));
}

static EfiFuncDef fdef_DatPool[] = {
	{ 0, &Type_DatPool, "_Ptr_ ()", f_null },
	{ 0, &Type_DatPool, "DatPool (int bsize = 0)", f_alloc },
	{ 0, &Type_DatPool, "DatPool (str path)", f_load },
	{ 0, &Type_void, "DatPool::save (str path)", f_save },
	{ 0, &Type_uint, "DatPool::add (. obj)", f_add },
	{ 0, &Type_obj, "DatPool::get (int, Type_t)", f_get },
};

void CmdSetup_DatPool(void)
{
	AddType(&Type_DatPool);
	AddFuncDef(fdef_DatPool, tabsize(fdef_DatPool));
}
