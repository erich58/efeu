/*
:*:regression parameters
:de:Regressionsparamweter

$Copyright (C) 1997 Erich Frühstück
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
#include <Math/TimeSeries.h>


/*	Datentype
*/

EfiType Type_OLSPar = REF_TYPE("OLSPar", OLSPar *);

/*	Komponentenfunktionen
*/

static EfiObj *par_nr (const EfiObj *obj, void *data)
{
	int x = obj ? Val_OLSPar(obj->data)->nr : 0;
	return NewObj(&Type_int, &x);
}

static EfiObj *par_df (const EfiObj *obj, void *data)
{
	int x = obj ? Val_OLSPar(obj->data)->df : 0;
	return NewObj(&Type_int, &x);
}

static EfiObj *par_r2 (const EfiObj *obj, void *data)
{
	double x = obj ? Val_OLSPar(obj->data)->r2 : 0;
	return NewObj(&Type_double, &x);
}

static EfiObj *par_rbar2 (const EfiObj *obj, void *data)
{
	double x = obj ? Val_OLSPar(obj->data)->rbar2 : 0;
	return NewObj(&Type_double, &x);
}

static EfiObj *par_dw (const EfiObj *obj, void *data)
{
	double x = obj ? Val_OLSPar(obj->data)->dw : 0;
	return NewObj(&Type_double, &x);
}

static EfiObj *par_see (const EfiObj *obj, void *data)
{
	double x = obj ? Val_OLSPar(obj->data)->see : 0;
	return NewObj(&Type_double, &x);
}

static EfiObj *par_base (const EfiObj *obj, void *data)
{
	TimeIndex x = obj ? Val_OLSPar(obj->data)->base : tindex_create(0, 0);
	return NewObj(&Type_TimeIndex, &x);
}

static EfiObj *par_first (const EfiObj *obj, void *data)
{
	TimeIndex x = obj ? Val_OLSPar(obj->data)->first : tindex_create(0, 0);
	return NewObj(&Type_TimeIndex, &x);
}

static EfiObj *par_last (const EfiObj *obj, void *data)
{
	TimeIndex x = obj ? Val_OLSPar(obj->data)->last : tindex_create(0, 0);
	return NewObj(&Type_TimeIndex, &x);
}


static EfiObj *par_koef (const EfiObj *obj, void *data)
{
	OLSPar *x = obj ? Val_OLSPar(obj->data) : NULL;

	if	(x == NULL)	return NULL;

	return EfiVecObj(&Type_OLSCoeff, x->koef, x->dim);
}

static EfiObj *par_exogen (const EfiObj *obj, void *data)
{
	OLSPar *x = obj ? Val_OLSPar(obj->data) : NULL;

	if	(x == NULL)	return NULL;

	return EfiVecObj(&Type_TimeSeries, x->exogen, x->dim);
}


static EfiObj *par_res (const EfiObj *obj, void *data)
{
	OLSPar *x = (obj ? Val_OLSPar(obj->data) : NULL);
	return x ? NewPtrObj(&Type_TimeSeries, rd_refer(x->res)) : NULL;
}

static EfiMember member[] = {
	{ "nr", &Type_int, par_nr, NULL },
	{ "df", &Type_int, par_df, NULL },
	{ "r2", &Type_double, par_r2, NULL },
	{ "rbar2", &Type_double, par_rbar2, NULL },
	{ "dw", &Type_double, par_dw, NULL },
	{ "see", &Type_double, par_see, NULL },
	{ "base", &Type_TimeIndex, par_base, NULL },
	{ "first", &Type_TimeIndex, par_first, NULL },
	{ "last", &Type_TimeIndex, par_last, NULL },
	{ "koef", &Type_vec, par_koef, NULL },
	{ "exogen", &Type_vec, par_exogen, NULL },
	{ "res", &Type_TimeSeries, par_res, NULL },
};


/*	Funktionen
*/

static void f_fprint (EfiFunc *func, void *rval, void **arg)
{
	IO *io = Val_io(arg[0]);
	OLSPar *par = Val_OLSPar(arg[1]);
	Val_int(rval) = par ? OLSPar_print(io, par) : io_puts("NULL", io);
}

static void f_symres (EfiFunc *func, void *rval, void **arg)
{
	OLSPar *par = Val_OLSPar(arg[0]);
	double *res;
	int i, n, k;

	if	(!par)	return;

	n = par->nr - 1;
	k = par->res->dim - par->nr;

	if	(k > n)	k = n;

	res = par->res->data;

	for (i = 1; i < n; i++)
	{
		if	(res[n - i] * res[n] <= 0.)
			break;

		res[n + i] = res[n - i];
	}
}

static void f_adjust (EfiFunc *func, void *rval, void **arg)
{
	OLSPar *par;
	double val, step;
	int n, k;

	par = Val_OLSPar(arg[0]);

	if	(!par)	return;

	k = tindex_diff(par->first, par->base) + par->nr;
	n = tindex_diff(par->first, Val_TimeIndex(arg[1]));

	if	(k < 1 || n <= k)	return;

	val = par->res->data[k - 1];
	step = -val / (n - k + 1.);

	if	(n > par->res->dim)	n = par->res->dim;

	while (k < n)
	{
		val += step;
		par->res->data[k++] = val;
	}

	while (k < par->res->dim)
		par->res->data[k++] = 0.;
}

/*	Initialisieren
*/

static EfiFuncDef func[] = {
	{ FUNC_VIRTUAL, &Type_OLSPar, "OLS (TimeSeries y, ...)",
		Func_OLS },
	{ FUNC_VIRTUAL, &Type_OLSPar, "OLS (TimeSeries y, List_t list)",
		Func_OLS },
	{ FUNC_VIRTUAL, &Type_TimeSeries,
		"OLSPar::symres ()", f_symres },
	{ FUNC_VIRTUAL, &Type_TimeSeries,
		"OLSPar::adjust (TimeIndex last)", f_adjust },
	{ FUNC_VIRTUAL, &Type_TimeSeries,
		"OLSPar::proj ()", Func_OLSProj },
	{ FUNC_VIRTUAL, &Type_TimeSeries,
		"OLSPar::proj (TimeIndex)", Func_OLSProj },
	{ FUNC_VIRTUAL, &Type_TimeSeries,
		"OLSPar::proj (TimeIndex, TimeIndex)", Func_OLSProj },
	{ FUNC_VIRTUAL, &Type_TimeSeries,
		"OLSPar::proj (TimeIndex, int n)", Func_OLSProj },
	{ FUNC_VIRTUAL, &Type_int, "fprint (IO, OLSPar)", f_fprint },
};


void CmdSetup_OLSPar (void)
{
	AddType(&Type_OLSPar);
	AddFuncDef(func, tabsize(func));
	AddEfiMember(Type_OLSPar.vtab, member, tabsize(member));
}
