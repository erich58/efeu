/*
:*:set up interpreter to use time series
:de:Befehlsinterpreter für Zeitreihenanalysen initialisieren

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

static size_t f_read (const EfiType *type, void *data, IO *io)
{
	Val_ptr(data) = ts_read(io);
	return Val_ptr(data) ? 1 : 0;
}

static size_t f_write (const EfiType *type, const void *data, IO *io)
{
	return ts_write(io, Val_ptr(data));
}

static int f_print (const EfiType *type, const void *data, IO *io)
{
	return ts_print(io, Val_ptr(data), TimeSeries_fmt);
}

EfiType Type_TimeSeries = IOREF_TYPE("TimeSeries", TimeSeries *,
	f_read, f_write, f_print);


/*	Komponentenfunktionen
*/

static EfiObj *ts_name (const EfiObj *obj, void *data)
{
	TimeSeries *ts = Val_ptr(obj->data);
	return ts ? LvalObj(&Lval_ref, &Type_str, ts, &ts->name) : str2Obj(NULL);
}

static EfiObj *ts_base (const EfiObj *obj, void *data)
{
	TimeSeries *ts = Val_ptr(obj->data);

	return ts ? LvalObj(&Lval_ref, &Type_TimeIndex, ts, &ts->base) :
		NewObj(&Type_TimeIndex, NULL);
}

static EfiObj *ts_last (const EfiObj *obj, void *data)
{
	TimeIndex x = { 0, 0 };
	TimeSeries *ts = Val_TimeSeries(obj->data);

	if	(ts)
	{
		x = ts->base;
		x.value += ts->dim - 1;
	}

	return NewObj(&Type_TimeIndex, &x);
}

static EfiObj *ts_dim (const EfiObj *obj, void *data)
{
	TimeSeries *ts = Val_TimeSeries(obj->data);
	int x = ts ? Val_TimeSeries(obj->data)->dim : 0;
	return NewObj(&Type_int, &x);
}

static EfiMember member[] = {
	{ "name", &Type_str, ts_name, NULL },
	{ "base", &Type_TimeIndex, ts_base, NULL },
	{ "first", &Type_TimeIndex, ts_base, NULL },
	{ "last", &Type_TimeIndex, ts_last, NULL },
	{ "dim", &Type_int, ts_dim, NULL },
};


/*	Funktionen
*/

/*
static void f_dummy (EfiFunc *func, void *rval, void **arg)
{
	Val_TimeSeries(rval) = NULL;
}
*/

static void f_xcreate (EfiFunc *func, void *rval, void **arg)
{
	TimeSeries *ts;
	EfiObjList *list;
	int i, dim;

	list = Val_list(arg[2]);
	dim = ObjListLen(list);
	ts = ts_create(Val_str(arg[0]), Val_TimeIndex(arg[1]), dim);

	for (i = 0; list != NULL; list = list->next)
		ts->data[i++] = Obj2double(RefObj(list->obj));

	Val_TimeSeries(rval) = ts;
}

static void f_ncreate (EfiFunc *func, void *rval, void **arg)
{
	TimeIndex base = Val_TimeIndex(arg[1]);
	int dim = 0;

	if	(func->arg[2].type == &Type_TimeIndex)
		dim = tindex_diff(base, Val_TimeIndex(arg[2])) + 1;
	else if	(func->arg[2].type == &Type_int)
		dim = Val_int(arg[2]);

	TimeSeries *ts = ts_create(Val_str(arg[0]), base, dim);

	double z = Val_double(arg[3]);
	double s = Val_double(arg[4]);

	for (int i = 0; i < ts->dim; i++, z += s)
		ts->data[i] = z;

	Val_TimeSeries(rval) = ts;
}

static void f_copy (EfiFunc *func, void *rval, void **arg)
{
	int n = 0;
	const char *name = "$1";

	if	(func->arg[0].type == &Type_str)
		name = Val_str(arg[n++]);

	TimeSeries *ts = Val_TimeSeries(arg[n++]);

	if	(ts != NULL)
	{
		int first, last;

		if	(n >= func->dim)
			first = 0;
		else
			first = tindex_diff(ts->base, Val_TimeIndex(arg[n++]));

		if	(n >= func->dim)
			last = ts->dim;
		else if	(func->arg[n].type == &Type_TimeIndex)
			last = tindex_diff(ts->base, Val_TimeIndex(arg[n])) + 1;
		else
			last = first + Val_int(arg[n]);

		ts = ts_copy(name, ts, first, last);
	}

	Val_TimeSeries(rval) = ts;
}

static void f_fprint (EfiFunc *func, void *rval, void **arg)
{
	Val_int(rval) = ts_print(Val_io(arg[0]), Val_TimeSeries(arg[1]),
		TimeSeries_fmt);
}

static void f_resize (EfiFunc *func, void *rval, void **arg)
{
	TimeSeries *ts = Val_TimeSeries(arg[0]);

	if	(ts == NULL)	return;

	int n = (int) ts->dim;

	if	(func->arg[1].type == &Type_TimeIndex)
		n = tindex_diff(ts->base, Val_TimeIndex(arg[1])) + 1;
	else if	(func->arg[1].type == &Type_int)
		n = Val_int(arg[1]);

	if	(n < 0)	n = 0;

	ts_resize(ts, n, Val_double(arg[2]));
	Val_TimeSeries(rval) = rd_refer(ts);
}

static void f_sync (EfiFunc *func, void *rval, void **arg)
{

	TimeSeries *ts = Val_TimeSeries(arg[0]);
	TimeIndex base = Val_TimeIndex(arg[1]);
	int dim = 0;

	if	(func->arg[2].type == &Type_TimeIndex)
		dim = tindex_diff(base, Val_TimeIndex(arg[2])) + 1;
	else if	(func->arg[2].type == &Type_int)
		dim = Val_int(arg[2]);

	ts_sync(ts, base, dim, Val_int(arg[3]));
	Val_TimeSeries(rval) = rd_refer(ts);
}

static void f_dim(EfiFunc *func, void *rval, void **arg)
{
	TimeSeries *x = Val_TimeSeries(arg[0]);
	Val_int(rval) = x ? x->dim : 0;
}

static void f_lshift(EfiFunc *func, void *rval, void **arg)
{
	TimeSeries *ts = Val_TimeSeries(arg[0]);

	if	(ts)
	{
		ts = ts_copy(NULL, ts, 0, ts->dim);
		ts->base.value -= Val_int(arg[1]);
	}

	Val_TimeSeries(rval) = ts;
}

static void f_rshift(EfiFunc *func, void *rval, void **arg)
{
	TimeSeries *ts = Val_TimeSeries(arg[0]);

	if	(ts)
	{
		ts = ts_copy(NULL, ts, 0, ts->dim);
		ts->base.value += Val_int(arg[1]);
	}

	Val_TimeSeries(rval) = ts;
}

static void f_diff(EfiFunc *func, void *rval, void **arg)
{
	Val_TimeSeries(rval) = ts_diff(Val_TimeSeries(arg[0]),
		Val_int(arg[1]), Val_vfunc(arg[2]));
}

static void f_cum(EfiFunc *func, void *rval, void **arg)
{
	ts_cumulate(Val_TimeSeries(arg[0]),
		Val_TimeSeries(arg[1]), Val_vfunc(arg[2]));
}

static void f_ma(EfiFunc *func, void *rval, void **arg)
{
	TimeSeries *ts = Val_TimeSeries(arg[0]);
	ts_ma(ts, Val_int(arg[1]));
	Val_TimeSeries(rval) = rd_refer(ts);
}

static void f_cma(EfiFunc *func, void *rval, void **arg)
{
	TimeSeries *ts = Val_TimeSeries(arg[0]);
	ts_cma(ts, Val_int(arg[1]));
	Val_TimeSeries(rval) = rd_refer(ts);
}

static void f_conv(EfiFunc *func, void *rval, void **arg)
{
	Val_TimeSeries(rval) = ts_convert(NULL, Val_TimeSeries(arg[0]),
		tindex_type(Val_str(arg[1])), Val_str(arg[2]));
}

static void tsfunc (EfiFunc *func, void *rval, void **arg,
	double (*eval) (double *tab, size_t dim))
{
	TimeSeries *ts;
	int first, last;
	
	ts = Val_TimeSeries(arg[0]);

	if	(ts == NULL)
	{
		Val_double(rval) = 0.;
		return;
	}

	if	(func->dim > 1)
	{
		first = tindex_diff(ts->base, Val_TimeIndex(arg[1]));
	}
	else	first = 0;

	if	(func->dim > 2)
	{
		last = tindex_diff(ts->base, Val_TimeIndex(arg[2])) + 1;
	}
	else	last = ts->dim;

	if	(first < 0)		first = 0;
	if	(last > ts->dim)	last = ts->dim;

	Val_double(rval) = (first < last) ? 
		eval(ts->data + first, last - first) : 0;
}

static double do_sum (double *tab, size_t dim)
{
	double s;

	for (s = 0.; dim-- > 0; tab++)
		s += *tab;

	return s;
}

static void f_sum (EfiFunc *func, void *rval, void **arg)
{
	tsfunc(func, rval, arg, do_sum);
}

static double do_qsum (double *tab, size_t dim)
{
	double s;

	for (s = 0.; dim-- > 0; tab++)
		s += *tab * *tab;

	return s;
}

static void f_qsum (EfiFunc *func, void *rval, void **arg)
{
	tsfunc(func, rval, arg, do_qsum);
}

static double do_min (double *tab, size_t dim)
{
	double s;

	for (s = *tab++, dim--; dim-- > 0; tab++)
		if (s > *tab) s = *tab;

	return s;
}

static void f_min (EfiFunc *func, void *rval, void **arg)
{
	tsfunc(func, rval, arg, do_min);
}


static double do_max (double *tab, size_t dim)
{
	double s;

	for (s = *tab++, dim--; dim-- > 0; tab++)
		if (s < *tab) s = *tab;

	return s;
}

static void f_max (EfiFunc *func, void *rval, void **arg)
{
	tsfunc(func, rval, arg, do_max);
}


static void f_ts2md (EfiFunc *func, void *rval, void **arg)
{
	Val_mdmat(rval) = TimeSeries2mdmat(Val_TimeSeries(arg[0]));
}

/*
static void TimeSeries2List (EfiFunc *func, void *rval, void **arg)
{
	EfiObjList *list, **ptr;
	TimeSeries *ts;
	int i;

	list = NULL;
	ptr = &list;
	ts = Val_TimeSeries(arg[0]);

	for (i = 0; i < ts->dim; i++)
	{
		*ptr = NewObjList(LvalObj(&Type_double, ts, ts->data + i));
		ptr = &(*ptr)->next;
	}

	Val_list(rval) = list;
}
*/

static void f_index (EfiFunc *func, void *rval, void **arg)
{
	TimeSeries *ts;
	int n;

	ts = Val_TimeSeries(arg[0]);

	if	(ts == NULL)
		n = 0;
	else if	(func->arg[1].type == &Type_TimeIndex)
		n = tindex_diff(ts->base, Val_TimeIndex(arg[1]));
	else	n = Val_int(arg[1]);

	if	(ts == NULL || n < 0 || n >= ts->dim)
	{
		log_note(NULL, "[TimeSeries:11]", NULL);

		if	(!func->type)
		{
			Val_obj(rval) = NULL;
		}
		else	Val_double(rval) = 0.;

		return;
	}

	if	(!func->type)
	{
		Val_obj(rval) = LvalObj(&Lval_ref,
			&Type_double, ts, ts->data + n);
	}
	else	Val_double(rval) = ts->data[n];
}

static void f_value(EfiFunc *func, void *rval, void **arg)
{
	EfiObjList *list;
	TimeSeries *ts;
	TimeIndex idx;
	double x;

	ts = Val_TimeSeries(arg[0]);

	if	(ts == NULL)
	{
		Val_double(rval) = 0.;
		return;
	}

	list = Val_list(arg[1]);
	idx = ts->base;
	x = 0.;

	if	(list)
	{
		Obj2Data(RefObj(list->obj), &Type_TimeIndex, &idx);

		if	(list->next)
			x = Obj2double(RefObj(list->next->obj));
	}

	Val_double(rval) = ts_value(ts, idx, x);
}

static void f_xfunc (EfiFunc *func, void *rval, void **arg)
{
	Val_TimeSeries(rval) = ts_func(Val_vfunc(arg[0]),
		Val_TimeSeries(arg[1]));
}

static void f_func (EfiFunc *func, void *rval, void **arg)
{
	Val_TimeSeries(rval) = ts_func(GetGlobalFunc(func->name),
		Val_TimeSeries(arg[0]));
}

static void f_assign (EfiFunc *func, void *rval, void **arg)
{
	ts_assign(GetTypeFunc(&Type_double, func->name),
		Val_TimeSeries(arg[0]), func->arg[1].type, arg[1]);
	Val_TimeSeries(rval) = rd_refer(Val_TimeSeries(arg[0]));
}

static void f_term (EfiFunc *func, void *rval, void **arg)
{
	Val_TimeSeries(rval) = ts_term(GetGlobalFunc(func->name),
		func->arg[0].type, arg[0], func->arg[1].type, arg[1]);
}

static void f_xterm (EfiFunc *func, void *rval, void **arg)
{
	Val_TimeSeries(rval) = ts_term(Val_vfunc(arg[0]),
		func->arg[1].type, arg[1], func->arg[2].type, arg[2]);
}

static void f_ExpSmoothing (EfiFunc *func, void *rval, void **arg)
{
	Val_TimeSeries(rval) = ExpSmoothing(NULL, Val_TimeSeries(arg[0]),
		Val_double(arg[1]), Val_double(arg[2]), Val_int(arg[3]));
}

static void f_ExpSmoothingError (EfiFunc *func, void *rval, void **arg)
{
	Val_double(rval) = ExpSmoothingError(Val_TimeSeries(arg[0]),
		Val_double(arg[1]), Val_double(arg[2]));
}

static void f_ExpSmoothingInitial (EfiFunc *func, void *rval, void **arg)
{
	Val_double(rval) = ExpSmoothingInitial(Val_TimeSeries(arg[0]),
		Val_double(arg[1]));
}

static void f_seasonal (EfiFunc *func, void *rval, void **arg)
{
	Val_TimeSeries(rval) = ts_seasonal("S($1)",
		Val_TimeSeries(arg[0]), Val_int(arg[1]));
}

static void f_xseasonal (EfiFunc *func, void *rval, void **arg)
{
	Val_TimeSeries(rval) = ts_seasonal(Val_str(arg[0]),
		Val_TimeSeries(arg[1]), Val_int(arg[2]));
}
		
/*	Initialisieren
*/

static EfiFuncDef funcdef[] = {
/*
	{ FUNC_RESTRICTED, &Type_list, "TimeSeries ()", TimeSeries2List },
*/
	{ FUNC_RESTRICTED, &Type_mdmat, "TimeSeries ()", f_ts2md },
	{ FUNC_VIRTUAL, &Type_int, "fprint (IO, TimeSeries)", f_fprint },
/*
	{ 0, &Type_TimeSeries, "TimeSeries ()", f_dummy },
*/

	{ 0, &Type_TimeSeries, "TimeSeries (TimeSeries x)", f_copy },
	{ 0, &Type_TimeSeries,
		"TimeSeries (TimeSeries x, \
TimeIndex a)", f_copy },
	{ 0, &Type_TimeSeries,
		"TimeSeries (TimeSeries x, \
TimeIndex a, int b)", f_copy },
	{ 0, &Type_TimeSeries,
		"TimeSeries (TimeSeries x, \
TimeIndex a, TimeIndex b)", f_copy },

	{ 0, &Type_TimeSeries, "TimeSeries (str name, "
		"TimeSeries x)", f_copy },
	{ 0, &Type_TimeSeries, "TimeSeries (str name, "
		"TimeSeries x, TimeIndex a)", f_copy },
	{ 0, &Type_TimeSeries, "TimeSeries (str name, "
		"TimeSeries x, TimeIndex a, int b)", f_copy },
	{ 0, &Type_TimeSeries, "TimeSeries (str name, "
		"TimeSeries x, TimeIndex a, TimeIndex b)", f_copy },

	{ 0, &Type_TimeSeries, "TimeSeries (str name, "
		"TimeIndex base, TimeIndex b, double x = 0., double s = 0.)",
		f_ncreate },
	{ 0, &Type_TimeSeries, "TimeSeries (str name, "
		"TimeIndex base, int b, double x = 0., double s = 0.)",
		f_ncreate },
	{ 0, &Type_TimeSeries,
		"TimeSeries (str name, TimeIndex base, ...)", f_xcreate },
	{ FUNC_VIRTUAL, &Type_TimeSeries,
		"operator<< (TimeSeries x, int n)", f_lshift },
	{ FUNC_VIRTUAL, &Type_TimeSeries, "operator>> (TimeSeries x, int n)",
		f_rshift },

	{ FUNC_VIRTUAL, &Type_int, "dim (TimeSeries)", f_dim },

	{ FUNC_VIRTUAL, &Type_double,
		"operator() (TimeSeries, List_t list)", f_value },
	{ FUNC_VIRTUAL, &Type_double,
		"operator[] (TimeSeries, TimeIndex)", f_index },
	{ FUNC_VIRTUAL, &Type_double,
		"operator[] (TimeSeries, int)", f_index },
	{ FUNC_VIRTUAL, &Type_obj,
		"operator[] (TimeSeries &, TimeIndex)", f_index },
	{ FUNC_VIRTUAL, &Type_obj,
		"operator[] (TimeSeries &, int)", f_index },

	{ FUNC_VIRTUAL, &Type_TimeSeries,
		"TimeSeries::resize(TimeIndex, double = 0.)", f_resize },
	{ FUNC_VIRTUAL, &Type_TimeSeries,
		"TimeSeries::resize(int, double = 0.)", f_resize },
	{ FUNC_VIRTUAL, &Type_TimeSeries, "TimeSeries::sync(TimeIndex, "
		"int dim, int offset = 0)", f_sync },
	{ FUNC_VIRTUAL, &Type_TimeSeries, "TimeSeries::sync(TimeIndex, "
		"TimeIndex, int offset = 0)", f_sync },

	{ FUNC_VIRTUAL, &Type_TimeSeries,
		"TimeSeries::diff(int lag, VirFunc = operator- )", f_diff },
	{ FUNC_VIRTUAL, &Type_void,
		"TimeSeries::cum(TimeSeries base, VirFunc = operator+ )", f_cum },
	{ FUNC_VIRTUAL, &Type_TimeSeries,
		"TimeSeries::ma (int n)", f_ma },
	{ FUNC_VIRTUAL, &Type_TimeSeries,
		"TimeSeries::cma (int n = 3)", f_cma },

	{ FUNC_VIRTUAL, &Type_TimeSeries,
		"TimeSeries::conv(str type, str flags = NULL)", f_conv },
		
	{ FUNC_VIRTUAL, &Type_double,
		"TimeSeries::sum ()", f_sum },
	{ FUNC_VIRTUAL, &Type_double,
		"TimeSeries::sum (TimeIndex)", f_sum },
	{ FUNC_VIRTUAL, &Type_double,
		"TimeSeries::sum (TimeIndex, TimeIndex)", f_sum },

	{ FUNC_VIRTUAL, &Type_double,
		"TimeSeries::qsum ()", f_qsum },
	{ FUNC_VIRTUAL, &Type_double,
		"TimeSeries::qsum (TimeIndex)", f_qsum },
	{ FUNC_VIRTUAL, &Type_double,
		"TimeSeries::qsum (TimeIndex, TimeIndex)", f_qsum },

	{ FUNC_VIRTUAL, &Type_double,
		"TimeSeries::min ()", f_min },
	{ FUNC_VIRTUAL, &Type_double,
		"TimeSeries::min (TimeIndex)", f_min },
	{ FUNC_VIRTUAL, &Type_double,
		"TimeSeries::min (TimeIndex, TimeIndex)", f_min },

	{ FUNC_VIRTUAL, &Type_double,
		"TimeSeries::max ()", f_max },
	{ FUNC_VIRTUAL, &Type_double,
		"TimeSeries::max (TimeIndex)", f_max },
	{ FUNC_VIRTUAL, &Type_double,
		"TimeSeries::max (TimeIndex, TimeIndex)", f_max },

/*	Zuweisungen
*/
	{ FUNC_VIRTUAL, &Type_TimeSeries,
		"TimeSeries::operator+= & (TimeSeries)", f_assign },
	{ FUNC_VIRTUAL, &Type_TimeSeries,
		"TimeSeries::operator+= & (double)", f_assign },
	{ FUNC_VIRTUAL, &Type_TimeSeries,
		"TimeSeries::operator-= & (TimeSeries)", f_assign },
	{ FUNC_VIRTUAL, &Type_TimeSeries,
		"TimeSeries::operator-= & (double)", f_assign },
	{ FUNC_VIRTUAL, &Type_TimeSeries,
		"TimeSeries::operator*= & (TimeSeries)", f_assign },
	{ FUNC_VIRTUAL, &Type_TimeSeries,
		"TimeSeries::operator*= & (double)", f_assign },
	{ FUNC_VIRTUAL, &Type_TimeSeries,
		"TimeSeries::operator/= & (TimeSeries)", f_assign },
	{ FUNC_VIRTUAL, &Type_TimeSeries,
		"TimeSeries::operator/= & (double)", f_assign },
	{ FUNC_VIRTUAL, &Type_TimeSeries, "ExpSmoothing (TimeSeries x, "
		"double alpha, double sw, int k = 0)", f_ExpSmoothing },
	{ FUNC_VIRTUAL, &Type_double, "ExpSmoothingError (TimeSeries x, "
		"double alpha, double sw)", f_ExpSmoothingError },
	{ FUNC_VIRTUAL, &Type_double, "ExpSmoothingInitial (TimeSeries x, "
		"double alpha)", f_ExpSmoothingInitial },
	{ FUNC_VIRTUAL, &Type_TimeSeries,
		"Seasonal (TimeSeries x, int p = 12)", f_seasonal },
	{ FUNC_VIRTUAL, &Type_TimeSeries,
		"Seasonal (str name, TimeSeries x, int p = 12)", f_xseasonal },

/*	Unäre Terme/Funktionen
*/
	{ FUNC_VIRTUAL, &Type_TimeSeries, "operator-() (TimeSeries)", f_func },
	{ FUNC_VIRTUAL, &Type_TimeSeries, "operator+() (TimeSeries)", f_func },
	{ FUNC_VIRTUAL, &Type_TimeSeries, "exp (TimeSeries)", f_func },
	{ FUNC_VIRTUAL, &Type_TimeSeries, "log (TimeSeries)", f_func },
	{ FUNC_VIRTUAL, &Type_TimeSeries, "xlog (TimeSeries)", f_func },
	{ FUNC_VIRTUAL, &Type_TimeSeries,
		"call (VirFunc, TimeSeries)", f_xfunc },

/*	Binäre Terme
*/
	{ FUNC_VIRTUAL, &Type_TimeSeries,
		"operator+ (TimeSeries, TimeSeries)", f_term },
	{ FUNC_VIRTUAL, &Type_TimeSeries,
		"operator+ (double, TimeSeries)", f_term },
	{ FUNC_VIRTUAL, &Type_TimeSeries,
		"operator+ (TimeSeries, double)", f_term },
	{ FUNC_VIRTUAL, &Type_TimeSeries,
		"operator- (TimeSeries, TimeSeries)", f_term },
	{ FUNC_VIRTUAL, &Type_TimeSeries,
		"operator- (double, TimeSeries)", f_term },
	{ FUNC_VIRTUAL, &Type_TimeSeries,
		"operator- (TimeSeries, double)", f_term },
	{ FUNC_VIRTUAL, &Type_TimeSeries,
		"operator* (TimeSeries, TimeSeries)", f_term },
	{ FUNC_VIRTUAL, &Type_TimeSeries,
		"operator* (double, TimeSeries)", f_term },
	{ FUNC_VIRTUAL, &Type_TimeSeries,
		"operator* (TimeSeries, double)", f_term },
	{ FUNC_VIRTUAL, &Type_TimeSeries,
		"operator/ (TimeSeries, TimeSeries)", f_term },
	{ FUNC_VIRTUAL, &Type_TimeSeries,
		"operator/ (TimeSeries, double)", f_term },
	{ FUNC_VIRTUAL, &Type_TimeSeries,
		"operator/ (double, TimeSeries)", f_term },

	{ FUNC_VIRTUAL, &Type_TimeSeries,
		"call (VirFunc, TimeSeries, TimeSeries)", f_xterm },
	{ FUNC_VIRTUAL, &Type_TimeSeries,
		"call (VirFunc, TimeSeries, double)", f_xterm },
	{ FUNC_VIRTUAL, &Type_TimeSeries,
		"call (VirFunc, double, TimeSeries)", f_xterm },
};

extern char *TimeSeries_fmt;

static EfiVarDef v_TimeSeries[] = {
	{ "fmt", &Type_str, &TimeSeries_fmt },
};

void CmdSetup_TimeSeries (void)
{
	AddType(&Type_TimeSeries);
	AddVarDef(Type_TimeSeries.vtab, v_TimeSeries, tabsize(v_TimeSeries));
	AddFuncDef(funcdef, tabsize(funcdef));
	AddEfiMember(Type_TimeSeries.vtab, member, tabsize(member));
}
