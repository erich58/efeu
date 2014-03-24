/*
Datensicht initialisieren

$Copyright (C) 2007 Erich Frühstück
This file is part of EFEU.

EFEU is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public
License as published by the Free Software Foundation; either
version 2 of the License, or (at your option) any later version.

EFEU is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty
of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU General Public License for more details.

You should have received a copy of the GNU General Public
License along with EFEU; see the file COPYING.
If not, write to the Free Software Foundation, Inc.,
59 Temple Place, Suite 330, Boston, MA 02111-1307, USA.
*/

#include <EFEU/EfiView.h>
#include <EFEU/EDB.h>

EfiType Type_EfiView = REF_TYPE("EfiView", EfiView *);

static EfiObj *get_data (const EfiObj *obj, void *data)
{
	EfiView *view = obj ? Val_ptr(obj->data) : NULL;
	return view ? RefObj(view->data) : NULL;
}

static EfiObj *get_weight (const EfiObj *obj, void *data)
{
	EfiView *view = obj ? Val_ptr(obj->data) : NULL;
	return view ? LvalObj(&Lval_ref, &Type_double,
		view, &view->weight) : NULL;
}

static EfiMember view_var[] = {
	{ "data", NULL, get_data, NULL,
		":*:data object\n"
		":de:Datenobjekt\n" },
	{ "weight", NULL, get_weight, NULL,
		":*:weight\n"
		":de:Gewicht\n" },
};

static void any_to_view (EfiFunc *func, void *rval, void **arg)
{
	Val_ptr(rval) = EfiView_create(RefObj(arg[0]), Val_str(arg[1]));
}

static void edb_to_view (EfiFunc *func, void *rval, void **arg)
{
	EDB *edb = Val_ptr(arg[0]);
	Val_ptr(rval) = EfiView_create(edb ? RefObj(edb->obj) : NULL,
		Val_str(arg[1]));
}

static void view_process (EfiFunc *func, void *rval, void **arg)
{
	EfiView_process(Val_ptr(arg[0]));
}


static void view_vardef (EfiFunc *func, void *rval, void **arg)
{
	EfiView_var(Val_ptr(arg[0]), Val_str(arg[1]));
}

static void view_show (EfiFunc *func, void *rval, void **arg)
{
	EfiView_show(Val_ptr(arg[0]), Val_io(arg[1]));
}

static EfiFuncDef view_func[] = {
	{ 0, &Type_EfiView, "EfiView (. & obj, str def = NULL)", any_to_view },
	{ 0, &Type_EfiView, "EfiView (EDB edb, str def = NULL)", edb_to_view },
	{ 0, &Type_EfiView, "EfiView::process()", view_process },
	{ 0, &Type_EfiView, "EfiView::set_var(str def)", view_vardef },
	{ 0, &Type_EfiView, "EfiView::show(IO = iostd)", view_show },
};

void SetupEfiView(void)
{
	static int setup_done = 0;

	if	(setup_done)	return;

	setup_done = 1;

	SetupStd();
	SetupEDB();
	AddType(&Type_EfiView);
	AddFuncDef(view_func, tabsize(view_func));
	AddEfiMember(Type_EfiView.vtab, view_var, tabsize(view_var));
	EfiViewSeq_setup();
	AddEfiPar(NULL, &EDBFilter_view);
	AddEfiPar(NULL, &EfiView_record);
	AddEfiPar(&Type_vec, &EfiView_element);
}
