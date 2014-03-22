/*
Stringpool

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
#include <EFEU/StrPool.h>

EfiType Type_StrPool = REF_TYPE("StrPool", StrPool *);

#define	POOL(x)	((StrPool **) (x))[0]

static void f_null (EfiFunc *func, void *rval, void **arg)
{
	POOL(rval) = NULL;
}

static void f_alloc (EfiFunc *func, void *rval, void **arg)
{
	POOL(rval) = NewStrPool();
}

static void f_load (EfiFunc *func, void *rval, void **arg)
{
	POOL(rval) = StrPool_map(Val_str(arg[0]));
}

static void f_add (EfiFunc *func, void *rval, void **arg)
{
	StrPool *pool = Val_ptr(arg[0]);
	*((uint64_t *) rval) = StrPool_add(pool, Val_str(arg[1]));
}

static void f_printf (EfiFunc *func, void *rval, void **arg)
{
	StrPool *pool = Val_ptr(arg[0]);
	IO *io = StrPool_open(pool);
	*((uint64_t *) rval) = StrPool_offset(pool);
	PrintFmtList(io, Val_str(arg[1]), Val_list(arg[2]));
	io_close(io);
}

static void f_get (EfiFunc *func, void *rval, void **arg)
{
	StrPool *pool = Val_ptr(arg[0]);
	EfiObj *idx = NULL;

	if	(sizeof(size_t) <= Type_uint32.size)
		idx = NewObj(&Type_uint32, NULL);
	else	idx = NewObj(&Type_uint64, NULL);

	*((size_t *) idx->data) = *((uint64_t *) arg[1]);
	Val_obj(rval) = StrPoolObj(idx, pool, idx->data);
}

static void f_next (EfiFunc *func, void *rval, void **arg)
{
	*((uint64_t *) rval) = StrPool_next(Val_ptr(arg[0]));
}

static void f_save (EfiFunc *func, void *rval, void **arg)
{
	StrPool_save(Val_ptr(arg[0]), Val_str(arg[1]));
}

static void f_dump (EfiFunc *func, void *rval, void **arg)
{
	StrPool_dump(Val_ptr(arg[0]), Val_ptr(arg[1]));
}

static void put_data (IO *out, char *data, size_t size, int last)
{
	size_t i;

	for (i = 0; i < size; i++)
	{
		if	(!last)
			io_puts("\\0\"\n\t\"", out);

		if	(data[i])
			io_xputc(data[i], out, "\"");

		last = data[i];
	}
}

static void f_cvar (EfiFunc *func, void *rval, void **arg)
{
	StrPool *pool = Val_ptr(arg[0]);
	IO *out = Val_ptr(arg[1]);
	char *name = Val_str(arg[2]);
	unsigned size = pool->csize + pool->used;

	if	(!pool || !out)	return;

	io_xprintf(out, "\nstatic char %s_data[%u] = \"", name, size);
	put_data(out, pool->cdata, pool->csize, 1);
	put_data(out, pool->mdata, pool->used, 0);
	io_puts("\";\n\n", out);

	io_xprintf(out, "static StrPool %s = {\n", name);
	io_puts("\tREFDATA(&StrPool_reftype), NULL,\n", out);
	io_xprintf(out, "\t%s_data, NULL,\n", name);
	io_xprintf(out, "\t%u, %u, 0, 0,\n", size, size);
	io_puts("};\n\n", out);
}

static EfiFuncDef fdef_StrPool[] = {
	{ 0, &Type_StrPool, "_Ptr_ ()", f_null },
	{ 0, &Type_StrPool, "StrPool (void)", f_alloc },
	{ 0, &Type_StrPool, "StrPool (str path)", f_load },
	{ 0, &Type_varsize, "StrPool::next ()", f_next },
	{ 0, &Type_varsize, "StrPool::add (str)", f_add },
	{ 0, &Type_varsize, "StrPool::printf (str fmt, ...)", f_printf },
	{ 0, &Type_void, "StrPool::save (str path)", f_save },
	{ 0, &Type_void, "StrPool::dump (IO = iostd )", f_dump },
	{ 0, &Type_void, "StrPool::cvar (IO out, str name)",
		f_cvar },
	{ FUNC_VIRTUAL, &Type_obj, "operator[] (StrPool, varsize)", f_get },
};

void CmdSetup_StrPool(void)
{
	AddType(&Type_StrPool);
	AddFuncDef(fdef_StrPool, tabsize(fdef_StrPool));
}
