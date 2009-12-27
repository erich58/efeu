/*
:*:variance data
:de:Varianzdaten

$Copyright (C) 2006 Erich Frühstück
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

#include <EFEU/StatData.h>
#include <EFEU/stdtype.h>
#include <EFEU/printobj.h>
#include <EFEU/mktype.h>

static int var_print (const EfiType *st, const void *data, IO *io)
{
	const VarianceData *var = data;
	return io_xprintf(io, "{ %.16g, %.16g, %.16g }",
		var->n, var->x, var->xx);
}

EfiType Type_VarianceData = EXTERN_TYPE("VarianceData", VarianceData,
	&Type_FrequencyData, var_print);

static EfiObj *member_mean (const EfiObj *base, void *data)
{
	VarianceData *v = base ? base->data : Type_VarianceData.defval;
	return double2Obj(v->x / v->n);
}

static EfiObj *member_var (const EfiObj *base, void *data)
{
	VarianceData *v = base ? base->data : Type_VarianceData.defval;
	return double2Obj((v->n > 1.) ? 
		(v->xx - v->x * v->x / v->n) / (v->n - 1.) :
		ExceptionValue(0.));
}

static EfiObj *member_s (const EfiObj *base, void *data)
{
	VarianceData *v = base ? base->data : Type_VarianceData.defval;
	return double2Obj((v->n > 1.) ? 
		sqrt((v->xx - v->x * v->x / v->n) / (v->n - 1.)) :
		ExceptionValue(0.));
}

static EfiMember vardef[] = {
	{ "mean", &Type_double, member_mean, NULL },
	{ "var", &Type_double, member_var, NULL },
	{ "s", &Type_double, member_s, NULL },
};

static void var_double (EfiFunc *func, void *rval, void **arg)
{
	VarianceData *var = rval;
	var->n = 1.;
	var->x = Val_double(arg[0]);
	var->xx = var->x * var->x;
}

static void var_list (EfiFunc *func, void *rval, void **arg)
{
	VarianceData *var = rval;
	EfiObjList *list = Val_ptr(arg[0]);
	var->n = list ? Obj2double(RefObj(list->obj)) : 0.;
	var->x = list->next ? Obj2double(RefObj(list->next->obj)) : 0.;
	var->xx = list->next && list->next->next ?
		Obj2double(RefObj(list->next->next->obj)) : var->x * var->x;
}

static void var_to_list (EfiFunc *func, void *rval, void **arg)
{
	VarianceData *var = arg[0];
	Val_ptr(rval) = MakeObjList(3, double2Obj(var->n),
		double2Obj(var->x), double2Obj(var->xx));
}

static void var_to_freq (EfiFunc *func, void *rval, void **arg)
{
	FrequencyData *freq = rval;
	VarianceData *var = arg[0];
	freq->n = var->n;
	freq->x = var->x;
}

static void var_fprint (EfiFunc *func, void *rval, void **arg)
{
	IO *out = Val_io(arg[0]);
	VarianceData *var = arg[1];
	int n = io_puts(PrintListBegin, out);
	n += PrintDouble(out, var->n);
	n += io_puts(PrintListDelim, out);
	n += PrintDouble(out, var->x);
	n += io_puts(PrintListDelim, out);
	n += PrintDouble(out, var->xx);
	n += io_puts(PrintListEnd, out);
	Val_int(rval) = n;
}

static void binary_add (EfiFunc *func, void *rval, void **arg)
{
	VarianceData *var = rval;
	VarianceData *a = arg[0];
	VarianceData *b = arg[1];
	var->n = a->n + b->n;
	var->x = a->x + b->x;
	var->xx = a->xx + b->xx;
}

static void assign_add (EfiFunc *func, void *rval, void **arg)
{
	VarianceData *a = arg[0];
	VarianceData *b = arg[1];
	a->n += b->n;
	a->x += b->x;
	a->xx += b->xx;
}


static EfiFuncDef func[] = {
	{ FUNC_VIRTUAL, &Type_VarianceData,
		"VarianceData (double)", var_double },
	{ FUNC_VIRTUAL, &Type_VarianceData,
		"VarianceData (List_t)", var_list },
	{ FUNC_VIRTUAL|FUNC_RESTRICTED, &Type_list,
		"VarianceData ()", var_to_list },
	{ FUNC_VIRTUAL|FUNC_RESTRICTED, &Type_FrequencyData,
		"VarianceData ()", var_to_freq },
	{ FUNC_VIRTUAL, &Type_int,
		"fprint (IO, VarianceData)", var_fprint },
	{ FUNC_VIRTUAL, &Type_VarianceData, \
		"operator+ (VarianceData, VarianceData)", binary_add },
	{ 0, &Type_VarianceData, \
		"& VarianceData::operator+= & (VarianceData)", assign_add },
};

void CmdSetup_VarianceData (void)
{
	static int setup_done = 0;
	EfiType *type = &Type_VarianceData;
	EfiStruct *st_var = type->list;

	if	(setup_done)	return;

	setup_done = 1;
	CmdSetup_FrequencyData();
	AddType(type);
	st_var = EfiType_stdvar(type, st_var, offsetof(VarianceData, n),
		0, "double", "n", "number of observations");
	st_var = EfiType_stdvar(type, st_var, offsetof(VarianceData, x),
		0, "double", "x", "aggregated values");
	st_var = EfiType_stdvar(type, st_var, offsetof(VarianceData, xx),
		0, "double", "xx", "sum of squares");
	AddEfiMember(type->vtab, vardef, tabsize(vardef));
	AddFuncDef(func, tabsize(func));
}
