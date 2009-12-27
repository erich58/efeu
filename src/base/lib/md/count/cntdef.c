/*
Zähldefinitionen für EFEU-Interpreter

$Copyright (C) 2003 Erich Frühstück
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

#include <EFEU/mdcount.h>
#include <EFEU/stdtype.h>
#include <EFEU/TimeRange.h>

EfiType Type_cotab = REF_TYPE ("CountTab", MdCountPar *);
EfiType Type_cltab = REF_TYPE ("ClassTab", MdCountPar *);
EfiType Type_cdef = REF_TYPE ("MdCntDef", MdCntDef *);
EfiType Type_cntobj = PTR_TYPE ("MdCount", MdCount *, &Type_ptr, NULL, NULL);

static int p_trange (const EfiType *st, const void *data, IO *io)
{
	return PrintTimeRange(io, (TimeRange *) data, 1);
}

EfiType Type_trange = SIMPLE_TYPE("TimeRange", TimeRange, NULL, p_trange);

static EfiObj *cdef_obj (const EfiObj *base, void *data)
{
	MdCntDef *def = base ? Val_ptr(base->data) : NULL;
	return def ? RefObj(def->obj) : NULL;
}

static EfiMember cdef_member[] = {
	{ "obj", NULL, cdef_obj, NULL, },
};

static EfiObj *rng_label (const EfiObj *base, void *par)
{
	char *label = base ? ((TimeRange *) base->data)->label : NULL;
	return str2Obj(mstrcpy(label));
}

static EfiObj *rng_ug (const EfiObj *base, void *par)
{
	int val = base ? ((TimeRange *) base->data)->ug : 0;
	return NewObj(&Type_Date, &val);
}

static EfiObj *rng_og (const EfiObj *base, void *par)
{
	int val = base ? ((TimeRange *) base->data)->og : 0;
	return NewObj(&Type_Date, &val);
}

static EfiObj *rng_dat (const EfiObj *base, void *par)
{
	int val = base ? ((TimeRange *) base->data)->dat : 0;
	return NewObj(&Type_Date, &val);
}

static EfiObj *rng_jahr (const EfiObj *base, void *par)
{
	int val = base ? ((TimeRange *) base->data)->jahr : 0;
	return NewObj(&Type_int, &val);
}

static EfiObj *rng_tage (const EfiObj *base, void *par)
{
	int val = base ? ((TimeRange *) base->data)->tage : 0;
	return NewObj(&Type_int, &val);
}

static EfiMember var_trange[] = {
	{ "label", &Type_str, rng_label, NULL },
	{ "ug", &Type_Date, rng_ug, NULL },
	{ "og", &Type_Date, rng_og, NULL },
	{ "dat", &Type_Date, rng_dat, NULL },
	{ "jahr", &Type_int, rng_jahr, NULL },
	{ "tage", &Type_int, rng_tage, NULL },
};

static void cot_create (EfiFunc *func, void *rval, void **arg)
{
	Val_ptr(rval) = MdCountPar_create();
	MdCount_add(Val_ptr(rval), stdcount, 1);
}

static void cot_index (EfiFunc *func, void *rval, void **arg)
{
	MdCountPar *tab = Val_ptr(arg[0]);
	int idx = Val_int(arg[1]);

	if	(tab && idx < tab->vtab.used)
	{
		MdCount **p = tab->vtab.data;
		Val_ptr(rval) = p[idx];
	}
	else	Val_ptr(rval) = NULL;
}

static void cot_search (EfiFunc *func, void *rval, void **arg)
{
	Val_ptr(rval) = MdCount_get(Val_ptr(arg[0]), Val_str(arg[1]));
}

static void co_print (EfiFunc *func, void *rval, void **arg)
{
	IO *io = Val_ptr(arg[0]);
	MdCount *co = Val_ptr(arg[1]);

	Val_int(rval) = co ? 
		io_xprintf(io, "%s\t%s\n", co->name, co->desc) :
		io_puts("NULL", io);
}

static void cot_list (EfiFunc *func, void *rval, void **arg)
{
	MdCountPar *tab = Val_ptr(arg[0]);
	IO *io = Val_ptr(arg[1]);

	if	(tab && io)
	{
		MdCount **p;
		size_t n;

		for (p = tab->vtab.data, n = tab->vtab.used; n-- > 0; p++)
			io_xprintf(io, "%s\t%s\n", (*p)->name, (*p)->desc);
	}
}

static void clt_create (EfiFunc *func, void *rval, void **arg)
{
	Val_ptr(rval) = MdCountPar_create();
}

static void clt_xcreate (EfiFunc *func, void *rval, void **arg)
{
	MdClass *cl;
	Label *label;

	cl = memalloc(sizeof(MdClass) + sizeof(Label));
	label = (void *) (cl + 1);

	label->name = mstrcpy(Val_str(arg[1]));
	label->desc = mstrcpy(Val_str(arg[2]));
	cl->name = mstrcpy(Val_str(arg[0]));
	cl->desc = label->desc;
	cl->dim = 1;
	cl->label = label;
	cl->classify = NULL;
		
	Val_ptr(rval) = MdCountPar_create();
	MdClass_add(Val_ptr(rval), cl, 1);
}

static void clt_list (EfiFunc *func, void *rval, void **arg)
{
	MdCountPar *tab = Val_ptr(arg[0]);
	IO *io = Val_ptr(arg[1]);

	if	(tab && io)
	{
		MdClass **p;
		size_t n;

		for (p = tab->ctab.data, n = tab->ctab.used; n-- > 0; p++)
			MdClassList(io, *p);
	}
}

static void axis_create (EfiFunc *func, void *rval, void **arg)
{
	MdCountPar *tab = Val_ptr(arg[0]);
	IO *io = Val_ptr(arg[1]);
	mdaxis **ptr;
	
	ptr = rval;
	*ptr = NULL;

	while ((*ptr = md_ctabaxis(io, tab)) != NULL)
		ptr = &(*ptr)->next;
}

static void cdef_create (EfiFunc *func, void *rval, void **arg)
{
	Val_ptr(rval) = MdCntDef_create(Val_type(arg[0]), Val_str(arg[1]));
}


static void m_create (EfiFunc *func, void *rval, void **arg)
{
	mdmat *md;
	md = new_mdmat();
	md->axis = Val_ptr(arg[1]);
	md_ctabinit(md, Val_ptr(arg[0]));
	Val_ptr(rval) = md;
}

static void m_count (EfiFunc *func, void *rval, void **arg)
{
	md_count(Val_mdmat(arg[0]), NULL);
}

static void print_trange (EfiFunc *func, void *rval, void **arg)
{
	Val_int(rval) = PrintTimeRange(Val_ptr(arg[0]),
		(TimeRange *) arg[1], 1);
}

static EfiFuncDef fdef[] = {
	{ 0, &Type_cotab, "CountTab (void)", cot_create },
	{ 0, &Type_void, "CountTab::list (IO io = iostd)", cot_list },

	{ FUNC_VIRTUAL, &Type_int, "fprint(IO, MdCount)", co_print },
	{ FUNC_VIRTUAL, &Type_cntobj, "operator[] (CountTab, int)",
		cot_index },
	{ FUNC_VIRTUAL, &Type_cntobj, "operator[] (CountTab, str)",
		cot_search },

	{ 0, &Type_cltab, "ClassTab (void)", clt_create },
	{ 0, &Type_cltab, "ClassTab (str name, str label, str desc = NULL)",
		clt_xcreate },
	{ 0, &Type_void, "ClassTab::list (IO io = iostd)", clt_list },
	{ 0, &Type_mdaxis, "mdaxis (ClassTab, IO)", axis_create },

	{ 0, &Type_cdef, "MdCntDef (Type_t type, str pfx = NULL)",
		cdef_create },
	{ FUNC_VIRTUAL, &Type_void, "MdCntDef::class (str name, str desc, "
		"promotion Expr_t exp, promotion IO def", MdCntDef_expr },
	{ FUNC_VIRTUAL, &Type_void, "MdCntDef::class (str name, str desc, "
		"promotion Expr_t exp, int n, str name, str lbl = NULL",
		MdCntDef_expr },
	{ FUNC_VIRTUAL, &Type_void,
		"MdCntDef::add (ClassTab cl)", MdCntDef_cadd },
	{ FUNC_VIRTUAL, &Type_void,
		"MdCntDef::add (ClassTab cl, str ext, . & obj)",
		MdCntDef_cadd },

	{ FUNC_VIRTUAL, &Type_int, "fprint(IO, TimeRange)", print_trange },

	{ 0, &Type_mdmat, "mdmat (restricted MdCount cobj, mdaxis x = NULL)",
		m_create },
	{ 0, &Type_void, "mdmat::count ()", m_count },
};

void MdSetup_count (void)
{
	AddType(&Type_cotab);
	AddType(&Type_cltab);
	AddType(&Type_cdef);
	AddEfiMember(Type_cdef.vtab, cdef_member, tabsize(cdef_member));
	AddType(&Type_cntobj);
	AddType(&Type_trange);
	AddEfiMember(Type_trange.vtab, var_trange, tabsize(var_trange));
	VarTab_xadd(NULL, "stdcount", NULL, NewPtrObj(&Type_cntobj, stdcount));
	AddFuncDef(fdef, tabsize(fdef));
}
