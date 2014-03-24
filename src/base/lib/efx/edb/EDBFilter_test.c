/*
:*:test filter
:de:Testfilter

$Copyright (C) 2005 Erich Frühstück
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

#include <EFEU/EDB.h>
#include <EFEU/EDBFilter.h>

typedef struct TEST TEST;

struct TEST {
	REFVAR;
	EDB *base;
	EfiType *type;
	EfiObj *expr;
	EfiObjList *list;
	int (*test) (TEST *test);
};

static void test_clean (void *data)
{
	TEST *par = data;
	rd_deref(par->base);
	UnrefObj(par->expr);
	DelObjList(par->list);
	memfree(par);
}

static RefType test_reftype = REFTYPE_INIT("EDB_TEST", NULL, test_clean);

static int test_bool (TEST *test)
{
	return Obj2bool(RefObj(test->expr));
}

static int test_read (EfiType *type, void *data, void *opaque_par)
{
	TEST *par = opaque_par;
	int rval = 0;

	PushVarTab(RefVarTab(par->base->obj->type->vtab),
		RefObj(par->base->obj));

	while (edb_read(par->base))
	{
		if	(Obj2bool(RefObj(par->expr)))
		{
			rval = 1;
			break;
		}
	}

	PopVarTab();
	return rval;
}

static EDB *test_create (EDBFilter *filter, EDB *base,
	const char *opt, const char *arg)
{
	TEST *par;
	EDB *edb;
	IO *io;

	par = memalloc(sizeof *par);
	par->base = rd_refer(base);

	io = io_cstr(opt);
	PushVarTab(RefVarTab(base->obj->type->vtab), RefObj(base->obj));
	par->expr = Parse_cmd(io);
	par->type = &Type_bool;
	par->list = NULL;
	par->test = test_bool;
	PopVarTab();
	io_close(io);

	edb = edb_share(base);
	edb->read = test_read;
	edb->ipar = rd_init(&test_reftype, par);
	return edb;
}


EDBFilter EDBFilter_test = EDB_FILTER(NULL,
	"test", "[expr]", test_create, NULL,
	":*:select records with test expression"
	":de:Datensätze mit Testausdruck selektieren"
);
