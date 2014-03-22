/*
Verfügbarmachung des Tabellenkonstruktion für den esh-Interpreter

$Copyright (C) 2008 Erich Frühstück
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

#include <EFEU/Tabular.h>
#include <EFEU/object.h>
#include <EFEU/cmdconfig.h>
#include <EFEU/Info.h>

#define	LBL_RNG	\
	":*:tabular construction" \
	":de:Tabellenkonstruktion"

EfiType Type_Tabular = REF_TYPE("Tabular", Tabular *);

#define	Val_Tabular(data)	((Tabular **) data)[0]

static EfiObj *tab_io (const EfiObj *obj, void *data)
{
	Tabular *tab = obj ? Val_ptr(obj->data) : NULL;
	return NewPtrObj(&Type_io, tab ? rd_refer(tab->io) : NULL);
}

static EfiMember vdef[] = {
	{ "io", &Type_io, tab_io, NULL },
};

static void f_null (EfiFunc *func, void *rval, void **arg)
{
	Val_Tabular(rval) = NULL;
}

static void f_create (EfiFunc *func, void *rval, void **arg)
{
	Val_Tabular(rval) = NewTabular(Val_str(arg[0]), Val_str(arg[1]));
}

static void f_newline (EfiFunc *func, void *rval, void **arg)
{
	Tabular_newline(Val_Tabular(arg[0]));
}

static void f_endline (EfiFunc *func, void *rval, void **arg)
{
	Tabular_endline(Val_Tabular(arg[0]));
}

static void f_entry (EfiFunc *func, void *rval, void **arg)
{
	Tabular_entry(Val_Tabular(arg[0]));
}

static void f_hline (EfiFunc *func, void *rval, void **arg)
{
	Tabular_hline(Val_Tabular(arg[0]), Val_int(arg[1]));
}

static void f_cline (EfiFunc *func, void *rval, void **arg)
{
	Tabular_cline(Val_Tabular(arg[0]), Val_int(arg[1]),
		Val_int(arg[2]), Val_int(arg[3]));
}

static void f_margin (EfiFunc *func, void *rval, void **arg)
{
	Tabular *tab = Val_Tabular(arg[0]);

	if	(tab)	tab->margin = Val_int(arg[1]);
}

static void f_multicol (EfiFunc *func, void *rval, void **arg)
{
	Tabular *tab;

	if	(!(tab = Val_Tabular(arg[0])))
		return;

	tab->buf.cdim = Val_int(arg[1]);
}

static void f_text (EfiFunc *func, void *rval, void **arg)
{
	Tabular *tab;
	TabularCol *col;
	TabularRow *row;

	if	(!(tab = Val_Tabular(arg[0])))
		return;

	tab->buf.lnum = Val_int(arg[1]);
	tab->buf.cpos = Val_int(arg[2]);
	row = Tabular_getrow(tab, tab->buf.lnum);
	col = Tabular_getcol(tab, tab->buf.cpos);
	tab->buf.row = *row;
	tab->buf.col = *col;
	tab->buf.cdim = 1;

	if	(Val_int(arg[4]))
		Tabular_multicol(tab, Val_int(arg[4]), Val_str(arg[5]));

	tab->buf.i_text = tab->text.pos;
	sb_puts(Val_str(arg[3]), &tab->text);
	Tabular_entry(tab);
}

static void f_print (EfiFunc *func, void *rval, void **arg)
{
	Tabular_print(Val_Tabular(arg[0]), Val_io(arg[1]));
}

static EfiFuncDef fdef[] = {
	{ FUNC_RESTRICTED, &Type_Tabular, "_Ptr_ ()", f_null },
	{ 0, &Type_Tabular, "Tabular (str coldef, str opt = NULL)", f_create },
	{ 0, &Type_void, "Tabular::newline ()", f_newline },
	{ 0, &Type_void, "Tabular::endline ()", f_endline },
	{ 0, &Type_void, "Tabular::entry ()", f_entry },
	{ 0, &Type_void, "Tabular::multicol (int num, str fmt)", f_multicol },
	{ 0, &Type_void, "Tabular::hline (int n = 1)", f_hline },
	{ 0, &Type_void, "Tabular::cline (int n, int c1, int c2)", f_cline },
	{ 0, &Type_void, "Tabular::margin (int margin)", f_margin },
	{ 0, &Type_void, "Tabular::print (IO out)", f_print },
	{ 0, &Type_void, "Tabular::text (int row, int col, str text, "
		"int cdim = 0, str fmt = NULL)", f_text },
};


/*	Initialisierungsfunktion
*/

void SetupTabular()
{
	static int setup_done = 0;

	if	(setup_done)	return;

	setup_done = 1;
	AddType(&Type_Tabular);
	AddEfiMember(Type_Tabular.vtab, vdef, tabsize(vdef));
	AddFuncDef(fdef, tabsize(fdef));
}
