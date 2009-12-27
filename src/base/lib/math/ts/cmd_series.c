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

EfiType Type_TimeSeries = IOREF_TYPE("TimeSeries", TimeSeries *,
	f_read, f_write);


static char *def_fmt = "%#10.2f";


/*	Komponentenfunktionen
*/

static EfiObj *ts_name (const EfiObj *obj, void *data)
{
	if	(obj)
	{
		TimeSeries *ts = Val_ptr(obj->data);
		return LvalObj(&Lval_ref, &Type_str, ts, &ts->name);
	}

	return NULL;
}

static EfiObj *ts_fmt (const EfiObj *obj, void *data)
{
	if	(obj)
	{
		TimeSeries *ts = Val_ptr(obj->data);
		return LvalObj(&Lval_ref, &Type_str, ts, &ts->fmt);
	}

	return NULL;
}


static EfiObj *ts_base (const EfiObj *obj, void *data)
{
	if	(obj)
	{
		TimeSeries *ts = Val_ptr(obj->data);
		return LvalObj(&Lval_ref, &Type_TimeIndex, ts, &ts->base);
	}

	return NULL;
}

static EfiObj *ts_last (const EfiObj *obj, void *data)
{
	TimeSeries *ts = Val_TimeSeries(obj->data);

	if	(ts)
	{
		TimeIndex x = ts->base;
		x.value += ts->dim - 1;
		return NewObj(&Type_TimeIndex, &x);
	}

	return NULL;
}

static EfiObj *ts_dim (const EfiObj *obj, void *data)
{
	int x = obj ? Val_TimeSeries(obj->data)->dim : 0;
	return NewObj(&Type_int, &x);
}

static EfiMember member[] = {
	{ "name", &Type_str, ts_name, NULL },
	{ "fmt", &Type_str, ts_fmt, NULL },
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


static int get_index(TimeIndex base, EfiFunc *func, void **arg, int n, int flg)
{
	if	(func->dim <= n)
		return 0;

	if	(func->arg[n].type == &Type_TimeIndex)
		return tindex_diff(base, Val_TimeIndex(arg[n])) + flg;

	return Val_int(arg[n]) + flg;
}

static void f_ncreate (EfiFunc *func, void *rval, void **arg)
{
	int dim;
	TimeIndex base;
	TimeSeries *ts;
	double z, s;
	int i;

	base = Val_TimeIndex(arg[1]);

	if	(func->arg[2].type == &Type_TimeIndex)
		dim = tindex_diff(base, Val_TimeIndex(arg[2])) + 1;
	else if	(func->arg[2].type == &Type_int)
		dim = Val_int(arg[2]);
	else	dim = 0;

	ts = ts_create(Val_str(arg[0]), base, dim);

	z = Val_double(arg[3]);
	s = Val_double(arg[4]);

	for (i = 0; i < ts->dim; i++, z += s)
		ts->data[i] = z;

	Val_TimeSeries(rval) = ts;
}

static void f_copy (EfiFunc *func, void *rval, void **arg)
{
	TimeSeries *ts = Val_TimeSeries(arg[0]);

	if	(ts != NULL)
	{
		int first = func->dim <= 1 ? 0 :
			tindex_diff(ts->base, Val_TimeIndex(arg[1]));
		int last = func->dim <= 2 ? ts->dim : 
			get_index(ts->base, func, arg, 2, 1);

		ts = ts_copy(ts->name, ts, first, last);
	}

	Val_TimeSeries(rval) = ts;
}

static void f_xcopy (EfiFunc *func, void *rval, void **arg)
{
	TimeSeries *ts = Val_TimeSeries(arg[1]);

	if	(ts != NULL)
	{
		int first = func->dim <= 2 ? 0 :
			tindex_diff(ts->base, Val_TimeIndex(arg[2]));
		int last = func->dim <= 3 ? ts->dim : 
			get_index(ts->base, func, arg, 3, 1);

		ts = ts_copy(Val_str(arg[0]), ts, first, last);
	}

	Val_TimeSeries(rval) = ts;
}

static void f_fprint (EfiFunc *func, void *rval, void **arg)
{
	Val_int(rval) = ts_print(Val_io(arg[0]), Val_TimeSeries(arg[1]),
		def_fmt);
}

static void f_expand (EfiFunc *func, void *rval, void **arg)
{
	TimeSeries *ts;
	double z;
	int i, n;

	ts = Val_TimeSeries(arg[0]);
	Val_TimeSeries(rval) = rd_refer(ts);

	if	(ts == NULL)	return;

	i = ts->dim;

	if	(func->arg[1].type == &Type_TimeIndex)
		n = tindex_diff(ts->base, Val_TimeIndex(arg[1])) + 1;
	else if	(func->arg[1].type == &Type_int)
		n = (int) ts->dim + Val_int(arg[1]);
	else	n = (int) ts->dim;

	if	(n < 0)	n = 0;

	ts_expand(ts, n);
	z = Val_double(arg[2]);

	while (i < ts->dim)
		ts->data[i++] = z;
}

static void f_texpand (EfiFunc *func, void *rval, void **arg)
{
	TimeSeries *ts;
	TimeSeries *x;
	TimeIndex base;
	int k, o, n, dim;

	ts = Val_TimeSeries(arg[0]);
	Val_TimeSeries(rval) = rd_refer(ts);
	x = Val_TimeSeries(arg[1]);

	if	(ts->base.type != x->base.type)
	{
		dbg_note(NULL, "[TimeSeries:23]", NULL); 
		return;
	}

	if	(x->base.value < ts->base.value)
	{
		base = x->base;
	}
	else	base = ts->base;

	k = ts->base.value - base.value;
	o = x->base.value - base.value;

	n = ts->dim + k;
	dim = x->dim + o;

	if	(dim < n)	dim = n;

	ts_sync(ts, base, dim, 0);

	while (k-- > 0)
		ts->data[k] = x->data[k];

	for (; n < dim; n++)
		ts->data[n] = x->data[n - o];
}

static void f_sync (EfiFunc *func, void *rval, void **arg)
{
	TimeSeries *ts;
	TimeIndex base;
	int dim;

	ts = Val_TimeSeries(arg[0]);
	base = Val_TimeIndex(arg[1]);

	if	(func->arg[2].type == &Type_TimeIndex)
		dim = tindex_diff(base, Val_TimeIndex(arg[2])) + 1;
	else if	(func->arg[2].type == &Type_int)
		dim = Val_int(arg[2]);
	else	dim = 0;

	ts_sync(ts, base, dim, (func->dim > 3) ? Val_int(arg[3]) : 0);
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
	Val_TimeSeries(rval) = ts_ma(Val_TimeSeries(arg[0]),
		Val_int(arg[1]), Val_int(arg[2]));
}

static void f_ima(EfiFunc *func, void *rval, void **arg)
{
	ts_ima(Val_TimeSeries(arg[0]),
		Val_TimeSeries(arg[1]), Val_int(arg[2]));
}


static void f_rename(EfiFunc *func, void *rval, void **arg)
{
	TimeSeries *ts = Val_TimeSeries(arg[0]);

	if	(ts)
	{
		memfree(ts->name);
		ts->name = mstrcpy(Val_str(arg[1]));
	}

	Val_TimeSeries(rval) = rd_refer(ts);
}

static void f_setfmt(EfiFunc *func, void *rval, void **arg)
{
	TimeSeries *ts = Val_TimeSeries(arg[0]);

	if	(ts)
	{
		memfree(ts->fmt);
		ts->fmt = mstrcpy(Val_str(arg[1]));
	}

	Val_TimeSeries(rval) = rd_refer(ts);
}

static void f_glm(EfiFunc *func, void *rval, void **arg)
{
	TimeSeries *ts;
	double g1, g2;
	int n, i, j;
	
	ts = Val_TimeSeries(arg[0]);
	Val_TimeSeries(rval) = rd_refer(ts);
	n = Val_int(arg[1]);

	if	(ts == NULL || n <= 1)	return;

	if	(n % 2 == 0)
	{
		n = n / 2;
		g2 = 0.5 / (double) n;
		g1 = 0.5 * g2;
	}
	else
	{
		n = (n - 1) / 2;
		g1 = g2 = 1. / (2. * n + 1.);
	}

	if	(ts->dim <= 2 * n)
	{
		for (i = 1; i < ts->dim; i++)
			ts->data[0] += ts->data[i];

		ts->data[0] /= (double) ts->dim;
		ts->dim = 1;
		return;
	}

	ts->dim -= 2 * n;
	ts->base.value += n;

	for (i = 0; i < ts->dim; i++)
	{
		ts->data[i] += ts->data[i + 2 * n];
		ts->data[i] *= g1;

		for (j = 1; j < n; j++)
			ts->data[i] += g2 * (ts->data[i + j] + ts->data[i + j + n]);
		ts->data[i] += g2 * ts->data[i + n];
	}
}

static void f_konv(EfiFunc *func, void *rval, void **arg)
{
	Val_TimeSeries(rval) = ts_convert(NULL, Val_TimeSeries(arg[0]),
		tindex_type(Val_str(arg[1])), Val_str(arg[2]));
}

#if	0
static void f_j2m(EfiFunc *func, void *rval, void **arg)
{
	TimeSeries *ts = Val_TimeSeries(arg[0]);
	TimeSeriesKonv_j2m(ts);
	Val_TimeSeries(rval) = rd_refer(ts);
}
#endif


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
		dbg_note(NULL, "[TimeSeries:11]", NULL);
		Buf_double = 0.;

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
		"TimeSeries x)", f_xcopy },
	{ 0, &Type_TimeSeries, "TimeSeries (str name, "
		"TimeSeries x, TimeIndex a)", f_xcopy },
	{ 0, &Type_TimeSeries, "TimeSeries (str name, "
		"TimeSeries x, TimeIndex a, int b)", f_xcopy },
	{ 0, &Type_TimeSeries, "TimeSeries (str name, "
		"TimeSeries x, TimeIndex a, TimeIndex b)", f_xcopy },

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
		"TimeSeries::expand(TimeIndex, double = 0.)", f_expand },
	{ FUNC_VIRTUAL, &Type_TimeSeries,
		"TimeSeries::expand(int, double = 0.)", f_expand },
	{ FUNC_VIRTUAL, &Type_TimeSeries,
		"TimeSeries::expand(TimeSeries)", f_texpand },
	{ FUNC_VIRTUAL, &Type_TimeSeries, "TimeSeries::sync(TimeIndex, "
		"int dim, int offset = 0)", f_sync },
	{ FUNC_VIRTUAL, &Type_TimeSeries, "TimeSeries::sync(TimeIndex, "
		"TimeIndex, int offset = 0)", f_sync },
	{ FUNC_VIRTUAL, &Type_TimeSeries,
		"TimeSeries::rename(str name = NULL)", f_rename },
	{ FUNC_VIRTUAL, &Type_TimeSeries,
		"TimeSeries::setfmt(str fmt = NULL)", f_setfmt },

	{ FUNC_VIRTUAL, &Type_TimeSeries,
		"TimeSeries::diff(int lag, VirFunc = operator- )", f_diff },
	{ FUNC_VIRTUAL, &Type_void,
		"TimeSeries::cum(TimeSeries base, VirFunc = operator+ )", f_cum },
	{ FUNC_VIRTUAL, &Type_TimeSeries,
		"TimeSeries::ma (int n, int adjust = 1)", f_ma },
	{ FUNC_VIRTUAL, &Type_void,
		"TimeSeries::ima (TimeSeries base, int adjust = 1)", f_ima },
	{ FUNC_VIRTUAL, &Type_TimeSeries,
		"TimeSeries::glm (int n = 3)", f_glm },
/*
	{ FUNC_VIRTUAL, &Type_TimeSeries, "TimeSeries::j2m()", f_j2m },
*/

	{ FUNC_VIRTUAL, &Type_TimeSeries,
		"TimeSeries::konv(str type, str flags = NULL)", f_konv },
		
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


void CmdSetup_TimeSeries (void)
{
	AddType(&Type_TimeSeries);
	AddFuncDef(funcdef, tabsize(funcdef));
	AddEfiMember(Type_TimeSeries.vtab, member, tabsize(member));
}
