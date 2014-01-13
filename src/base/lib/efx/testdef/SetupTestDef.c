/*
:*:	setting up test definitions
:de:	Initialisierung der Testdefinitionen

$Copyright (C) 2002, 2005 Erich Frühstück
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

#include <EFEU/TestDef.h>
#include <EFEU/stdtype.h>

EfiType Type_TestDef = REF_TYPE("TestDef", TestDef *);

static int test_val (void *data, const EfiObj *obj)
{
	return TestVal(data, Obj2uint(RefObj(obj)));
}

static void par_num (EfiFunc *func, void *rval, void **arg)
{
	Val_ptr(rval) = test_create(&TestPar_Num, test_val,
		NULL, Val_str(arg[0]));
}

static void par_dat (EfiFunc *func, void *rval, void **arg)
{
	Val_ptr(rval) = test_create(&TestPar_Dat, test_val,
		NULL, Val_str(arg[0]));
}

static void par_dat1900 (EfiFunc *func, void *rval, void **arg)
{
	Val_ptr(rval) = test_create(&TestPar_Dat1900, test_val,
		NULL, Val_str(arg[0]));
}

static int test_code (void *data, const EfiObj *obj)
{
	return TestCode(data, Obj2uint(RefObj(obj)));
}

static void fpar_enum (EfiFunc *func, void *rval, void **arg)
{
	Val_ptr(rval) = test_create(&TestPar_enum, test_code,
		Val_str(arg[0]), Val_str(arg[1]));
}

static void f_create (EfiFunc *func, void *rval, void **arg)
{
	Val_ptr(rval) = TestDef_create(Val_type(arg[0]),
		func->dim > 1 ? Val_str(arg[1]) : NULL);
}

static void f_test (EfiFunc *func, void *rval, void **arg)
{
	TestDef *def = Val_ptr(arg[0]);
	EfiObjList *list = Val_list(arg[1]);

	for (Val_int(rval) = 0; list; list = list->next)
	{
		if	(TestDef_obj(def, list->obj))
		{
			Val_int(rval) = 1;
			break;
		}
	}
}
	
static EfiFuncDef fdef[] = {
	{ 0, &Type_TestDef, "TestDef (Type_t type)", f_create },
	{ 0, &Type_TestDef, "TestDef (Type_t type, str def)", f_create },
	{ 0, &Type_TestDef, "TestNum (str arg)", par_num },
	{ 0, &Type_TestDef, "TestDat (str arg)", par_dat },
	{ 0, &Type_TestDef, "TestDat1900 (str arg)", par_dat1900 },
	{ 0, &Type_TestDef, "TestEnum (str type, str arg)", fpar_enum },
	{ FUNC_VIRTUAL, &Type_bool, "operator() (TestDef, List_t)", f_test },
};

void SetupTestDef (void)
{
	static int setup_done = 0;

	if	(setup_done)	return;

	setup_done = 1;

	AddType(&Type_TestDef);
	AddFuncDef(fdef, tabsize(fdef));
}
