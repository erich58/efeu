/*
Befehlsinterpreter für Zeitreihenanalysen initialisieren

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

Type_t Type_TimeSeries = REF_TYPE("TimeSeries", TimeSeries_t *);


static char *def_name = NULL;
static char *def_fmt = "%#10.2f";
static TimeIndex_t def_base = { 0, 0 };


/*	Komponentenfunktionen
*/

static char **ts_name (TimeSeries_t **ts)
{
	return *ts ? &(*ts)->name : NULL;
}

static TimeIndex_t *ts_base (TimeSeries_t **ts)
{
	return *ts ? &(*ts)->base : NULL;
}

static TimeIndex_t Buf_TimeIndex = { 0, 0 };

static TimeIndex_t *ts_first (TimeSeries_t **ts)
{
	if	(*ts && (*ts)->dim)
	{
		Buf_TimeIndex = (*ts)->base;
		return &Buf_TimeIndex;
	}
	else	return NULL;
}

static TimeIndex_t *ts_last (TimeSeries_t **ts)
{
	if	(*ts && (*ts)->dim)
	{
		Buf_TimeIndex = (*ts)->base;
		Buf_TimeIndex.value += (*ts)->dim - 1;
		return &Buf_TimeIndex;
	}
	else	return NULL;
}

static char **ts_fmt (TimeSeries_t **ts)
{
	return *ts ? &(*ts)->fmt : NULL;
}

static int *ts_dim (TimeSeries_t **ts)
{
	Buf_int = *ts ? (*ts)->dim : 0;
	return &Buf_int;
}

static Var_t var_TimeSeries[] = {
	{ "name", &Type_str, &def_name, 0, 0, LvalMember, ts_name },
	{ "fmt", &Type_str, &def_fmt, 0, 0, LvalMember, ts_fmt },
	{ "base", &Type_TimeIndex, &def_base, 0, 0, LvalMember, ts_base },
	{ "first", &Type_TimeIndex, NULL, 0, 0, ConstMember, ts_first },
	{ "last", &Type_TimeIndex, NULL, 0, 0, ConstMember, ts_last },
	{ "dim", &Type_int, NULL, 0, 0, ConstMember, ts_dim },
};


/*	Funktionen
*/

/*
static void f_dummy (Func_t *func, void *rval, void **arg)
{
	Val_TimeSeries(rval) = NULL;
}
*/

static void f_xcreate (Func_t *func, void *rval, void **arg)
{
	TimeSeries_t *ts;
	ObjList_t *list;
	int i, dim;

	list = Val_list(arg[2]);
	dim = ObjListLen(list);
	ts = NewTimeSeries(Val_str(arg[0]), Val_TimeIndex(arg[1]), dim);

	for (i = 0; list != NULL; list = list->next)
		ts->data[i++] = Obj2double(RefObj(list->obj));

	Val_TimeSeries(rval) = ts;
}


static int get_index(TimeIndex_t base, Func_t *func, void **arg, int n, int flg)
{
	if	(func->dim <= n)
		return 0;

	if	(func->arg[n].type == &Type_TimeIndex)
		return DiffTimeIndex(base, Val_TimeIndex(arg[n])) + flg;

	return Val_int(arg[n]) + flg;
}

static void f_ncreate (Func_t *func, void *rval, void **arg)
{
	int dim;
	TimeIndex_t base;
	TimeSeries_t *ts;
	double z;
	int i;

	base = Val_TimeIndex(arg[1]);

	if	(func->arg[2].type == &Type_TimeIndex)
		dim = DiffTimeIndex(base, Val_TimeIndex(arg[2])) + 1;
	else if	(func->arg[2].type == &Type_int)
		dim = Val_int(arg[2]);
	else	dim = 0;

	ts = NewTimeSeries(Val_str(arg[0]), base, dim);

	z = Val_double(arg[3]);

	for (i = 0; i < ts->dim; i++)
		ts->data[i] = z;

	Val_TimeSeries(rval) = ts;
}

static void f_copy (Func_t *func, void *rval, void **arg)
{
	TimeSeries_t *ts = Val_TimeSeries(arg[0]);

	if	(ts != NULL)
	{
		int first = func->dim <= 1 ? 0 :
			DiffTimeIndex(ts->base, Val_TimeIndex(arg[1]));
		int last = func->dim <= 2 ? ts->dim : 
			get_index(ts->base, func, arg, 2, 1);

		ts = CopyTimeSeries(ts->name, ts, first, last);
	}

	Val_TimeSeries(rval) = ts;
}

static void f_xcopy (Func_t *func, void *rval, void **arg)
{
	TimeSeries_t *ts = Val_TimeSeries(arg[1]);

	if	(ts != NULL)
	{
		int first = func->dim <= 2 ? 0 :
			DiffTimeIndex(ts->base, Val_TimeIndex(arg[2]));
		int last = func->dim <= 3 ? ts->dim : 
			get_index(ts->base, func, arg, 3, 1);

		ts = CopyTimeSeries(Val_str(arg[0]), ts, first, last);
	}

	Val_TimeSeries(rval) = ts;
}


static void f_fprint (Func_t *func, void *rval, void **arg)
{
	Val_int(rval) = PrintTimeSeries(Val_io(arg[0]), Val_TimeSeries(arg[1]),
		def_fmt);
}

static void f_expand (Func_t *func, void *rval, void **arg)
{
	TimeSeries_t *ts;
	double z;
	int i, n;

	ts = Val_TimeSeries(arg[0]);
	Val_TimeSeries(rval) = rd_refer(ts);

	if	(ts == NULL)	return;

	i = ts->dim;

	if	(func->arg[1].type == &Type_TimeIndex)
		n = DiffTimeIndex(ts->base, Val_TimeIndex(arg[1])) + 1;
	else if	(func->arg[1].type == &Type_int)
		n = (int) ts->dim + Val_int(arg[1]);
	else	n = (int) ts->dim;

	if	(n < 0)	n = 0;

	ExpandTimeSeries(ts, n);
	z = Val_double(arg[2]);

	while (i < ts->dim)
		ts->data[i++] = z;
}

#if	0
static void f_sync (Func_t *func, void *rval, void **arg)
{
	TimeSeries_t *ts;
	double z;
	int i, n;

	ts = Val_TimeSeries(arg[0]);
	Val_TimeSeries(rval) = rd_refer(ts);

	if	(ts == NULL)	return;

	n = DiffTimeIndex(ts->base, Val_TimeIndex(arg[1]));
	z = Val_double(arg[2]);

	if	(n > 0)
	{
		ts->base.value += n;

		if	(n < ts->dim)
		{
			ts->dim -= n;

			for (i = 0; i < ts->dim; i++)
				ts->data[i] = ts->data[i + n];
		}
		else	ts->dim = 0;
	}
	else
	{
		n = -n;
		ts->base.value += n; 
		ExpandTimeSeries(ts, ts->dim + n);

		for (i = ts->dim; i >= n; i--)
			ts->data[i] = ts->data[i - n];

		for (; i >= 0; i--)
			ts->data[i] = z;
	}
}
#endif

static void f_sync (Func_t *func, void *rval, void **arg)
{
	TimeSeries_t *ts;
	TimeIndex_t base;
	int dim;

	ts = Val_TimeSeries(arg[0]);
	base = Val_TimeIndex(arg[1]);

	if	(func->arg[2].type == &Type_TimeIndex)
		dim = DiffTimeIndex(base, Val_TimeIndex(arg[2])) + 1;
	else if	(func->arg[2].type == &Type_int)
		dim = Val_int(arg[2]);
	else	dim = 0;

	SyncTimeSeries(ts, base, dim);
	Val_TimeSeries(rval) = rd_refer(ts);
}

static void f_dim(Func_t *func, void *rval, void **arg)
{
	TimeSeries_t *x = Val_TimeSeries(arg[0]);
	Val_int(rval) = x ? x->dim : 0;
}

static void f_lshift(Func_t *func, void *rval, void **arg)
{
	TimeSeries_t *ts = Val_TimeSeries(arg[0]);

	if	(ts)
	{
		ts = CopyTimeSeries(NULL, ts, 0, ts->dim);
		ts->base.value -= Val_int(arg[1]);
	}

	Val_TimeSeries(rval) = ts;
}

static void f_rshift(Func_t *func, void *rval, void **arg)
{
	TimeSeries_t *ts = Val_TimeSeries(arg[0]);

	if	(ts)
	{
		ts = CopyTimeSeries(NULL, ts, 0, ts->dim);
		ts->base.value += Val_int(arg[1]);
	}

	Val_TimeSeries(rval) = ts;
}

static void f_diff(Func_t *func, void *rval, void **arg)
{
	Val_TimeSeries(rval) = DiffTimeSeries(Val_TimeSeries(arg[0]),
		Val_int(arg[1]), Val_vfunc(arg[2]));
}

static void f_cum(Func_t *func, void *rval, void **arg)
{
	CumulateTimeSeries(Val_TimeSeries(arg[0]),
		Val_TimeSeries(arg[1]), Val_vfunc(arg[2]));
}


static void f_rename(Func_t *func, void *rval, void **arg)
{
	TimeSeries_t *ts = Val_TimeSeries(arg[0]);

	if	(ts)
	{
		memfree(ts->name);
		ts->name = mstrcpy(Val_str(arg[1]));
	}

	Val_TimeSeries(rval) = rd_refer(ts);
}

static void f_setfmt(Func_t *func, void *rval, void **arg)
{
	TimeSeries_t *ts = Val_TimeSeries(arg[0]);

	if	(ts)
	{
		memfree(ts->fmt);
		ts->fmt = mstrcpy(Val_str(arg[1]));
	}

	Val_TimeSeries(rval) = rd_refer(ts);
}

static void f_glm(Func_t *func, void *rval, void **arg)
{
	TimeSeries_t *ts;
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


static void f_konv(Func_t *func, void *rval, void **arg)
{
	Val_TimeSeries(rval) = KonvTimeSeries(NULL, Val_TimeSeries(arg[0]),
		TimeIndexType(Val_str(arg[1])), Val_str(arg[2]));
}

#if	0
static void f_j2m(Func_t *func, void *rval, void **arg)
{
	TimeSeries_t *ts = Val_TimeSeries(arg[0]);
	TimeSeriesKonv_j2m(ts);
	Val_TimeSeries(rval) = rd_refer(ts);
}
#endif


static void f_sum(Func_t *func, void *rval, void **arg)
{
	double s;
	TimeSeries_t *ts;
	int i, first, last;
	
	ts = Val_TimeSeries(arg[0]);
	s = 0;

	if	(ts == NULL)
	{
		Val_double(rval) = 0.;
		return;
	}

	if	(func->dim > 1)
	{
		first = DiffTimeIndex(ts->base, Val_TimeIndex(arg[1]));
	}
	else	first = 0;

	if	(func->dim > 2)
	{
		last = DiffTimeIndex(ts->base, Val_TimeIndex(arg[2])) + 1;
	}
	else	last = ts->dim;

	if	(first < 0)		first = 0;
	if	(last > ts->dim)	last = ts->dim;

	for (i = first; i < last; i++)
		s += ts->data[i];

	Val_double(rval) = s;
}

static void f_ts2md (Func_t *func, void *rval, void **arg)
{
	Val_mdmat(rval) = TimeSeries2mdmat(Val_TimeSeries(arg[0]));
}

/*
static void TimeSeries2List (Func_t *func, void *rval, void **arg)
{
	ObjList_t *list, **ptr;
	TimeSeries_t *ts;
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

static void f_index (Func_t *func, void *rval, void **arg)
{
	TimeSeries_t *ts;
	int n;

	ts = Val_TimeSeries(arg[0]);

	if	(ts == NULL)
		n = 0;
	else if	(func->arg[1].type == &Type_TimeIndex)
		n = DiffTimeIndex(ts->base, Val_TimeIndex(arg[1]));
	else	n = Val_int(arg[1]);

	if	(ts == NULL || n < 0 || n >= ts->dim)
	{
		errmsg(MSG_TS, 11);
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

static void f_value(Func_t *func, void *rval, void **arg)
{
	ObjList_t *list;
	TimeSeries_t *ts;
	TimeIndex_t idx;
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

	Val_double(rval) = TimeSeriesValue(ts, idx, x);
}

static void f_xfunc (Func_t *func, void *rval, void **arg)
{
	Val_TimeSeries(rval) = TimeSeriesFunc(Val_vfunc(arg[0]),
		Val_TimeSeries(arg[1]));
}

static void f_func (Func_t *func, void *rval, void **arg)
{
	Val_TimeSeries(rval) = TimeSeriesFunc(GetGlobalFunc(func->name),
		Val_TimeSeries(arg[0]));
}

static void f_assign (Func_t *func, void *rval, void **arg)
{
	AssignTimeSeries(GetTypeFunc(&Type_double, func->name),
		Val_TimeSeries(arg[0]), func->arg[1].type, arg[1]);
	Val_TimeSeries(rval) = rd_refer(Val_TimeSeries(arg[0]));
}

static void f_term (Func_t *func, void *rval, void **arg)
{
	Val_TimeSeries(rval) = TimeSeriesTerm(GetGlobalFunc(func->name),
		func->arg[0].type, arg[0], func->arg[1].type, arg[1]);
}

static void f_xterm (Func_t *func, void *rval, void **arg)
{
	Val_TimeSeries(rval) = TimeSeriesTerm(Val_vfunc(arg[0]),
		func->arg[1].type, arg[1], func->arg[2].type, arg[2]);
}


		
/*	Initialisieren
*/

static FuncDef_t ts_func[] = {
/*
	{ FUNC_RESTRICTED, &Type_list, "TimeSeries ()", TimeSeries2List },
*/
	{ FUNC_RESTRICTED, &Type_mdmat, "TimeSeries ()", f_ts2md },
	{ FUNC_VIRTUAL, &Type_int, "fprint (IO, TimeSeries)", f_fprint },
/*
	{ 0, &Type_TimeSeries, "TimeSeries ()", f_dummy },
*/

	{ 0, &Type_TimeSeries, "TimeSeries (TimeSeries x)", f_copy },
	{ 0, &Type_TimeSeries, "TimeSeries (TimeSeries x, \
TimeIndex a)", f_copy },
	{ 0, &Type_TimeSeries, "TimeSeries (TimeSeries x, \
TimeIndex a, int b)", f_copy },
	{ 0, &Type_TimeSeries, "TimeSeries (TimeSeries x, \
TimeIndex a, TimeIndex b)", f_copy },

	{ 0, &Type_TimeSeries, "TimeSeries (str name, TimeSeries x)", f_xcopy },
	{ 0, &Type_TimeSeries, "TimeSeries (str name, TimeSeries x, \
TimeIndex a)", f_xcopy },
	{ 0, &Type_TimeSeries, "TimeSeries (str name, TimeSeries x, \
TimeIndex a, int b)", f_xcopy },
	{ 0, &Type_TimeSeries, "TimeSeries (str name, TimeSeries x, \
TimeIndex a, TimeIndex b)", f_xcopy },

	{ 0, &Type_TimeSeries, "TimeSeries (str name, TimeIndex base, \
TimeIndex b, double x = 0.)", f_ncreate },
	{ 0, &Type_TimeSeries, "TimeSeries (str name, TimeIndex base, \
int b, double x = 0.)", f_ncreate },
	{ 0, &Type_TimeSeries, "TimeSeries (str name, TimeIndex base, ...)",
		f_xcreate },
	{ FUNC_VIRTUAL, &Type_TimeSeries, "operator<< (TimeSeries x, int n)",
		f_lshift },
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
		"TimeSeries::sync(TimeIndex)", f_sync },
	{ FUNC_VIRTUAL, &Type_TimeSeries,
		"TimeSeries::sync(TimeIndex, int dim)", f_sync },
	{ FUNC_VIRTUAL, &Type_TimeSeries,
		"TimeSeries::sync(TimeIndex, TimeIndex)", f_sync },
	{ FUNC_VIRTUAL, &Type_TimeSeries,
		"TimeSeries::rename(str name = NULL)", f_rename },
	{ FUNC_VIRTUAL, &Type_TimeSeries,
		"TimeSeries::setfmt(str fmt = NULL)", f_setfmt },

	{ FUNC_VIRTUAL, &Type_TimeSeries,
		"TimeSeries::diff(int lag, VirFunc = operator- )", f_diff },
	{ FUNC_VIRTUAL, &Type_void,
		"TimeSeries::cum(TimeSeries base, VirFunc = operator+ )", f_cum },
	{ FUNC_VIRTUAL, &Type_TimeSeries,
		"TimeSeries::glm(int n = 3)", f_glm },
/*
	{ FUNC_VIRTUAL, &Type_TimeSeries, "TimeSeries::j2m()", f_j2m },
*/

	{ FUNC_VIRTUAL, &Type_TimeSeries,
		"TimeSeries::konv(str type, str flags = NULL)", f_konv },
		

	{ FUNC_VIRTUAL, &Type_double, "TimeSeries::sum()", f_sum },
	{ FUNC_VIRTUAL, &Type_double, "TimeSeries::sum(TimeIndex)", f_sum },
	{ FUNC_VIRTUAL, &Type_double, "TimeSeries::sum(TimeIndex, TimeIndex)", f_sum },

/*	Zuweisungen
*/
	{ FUNC_VIRTUAL, &Type_TimeSeries, "TimeSeries::operator+= & (TimeSeries)", f_assign },
	{ FUNC_VIRTUAL, &Type_TimeSeries, "TimeSeries::operator+= & (double)", f_assign },
	{ FUNC_VIRTUAL, &Type_TimeSeries, "TimeSeries::operator-= & (TimeSeries)", f_assign },
	{ FUNC_VIRTUAL, &Type_TimeSeries, "TimeSeries::operator-= & (double)", f_assign },
	{ FUNC_VIRTUAL, &Type_TimeSeries, "TimeSeries::operator*= & (TimeSeries)", f_assign },
	{ FUNC_VIRTUAL, &Type_TimeSeries, "TimeSeries::operator*= & (double)", f_assign },
	{ FUNC_VIRTUAL, &Type_TimeSeries, "TimeSeries::operator/= & (TimeSeries)", f_assign },
	{ FUNC_VIRTUAL, &Type_TimeSeries, "TimeSeries::operator/= & (double)", f_assign },

/*	Unäre Terme/Funktionen
*/
	{ FUNC_VIRTUAL, &Type_TimeSeries, "operator-() (TimeSeries)", f_func },
	{ FUNC_VIRTUAL, &Type_TimeSeries, "operator+() (TimeSeries)", f_func },
	{ FUNC_VIRTUAL, &Type_TimeSeries, "exp (TimeSeries)", f_func },
	{ FUNC_VIRTUAL, &Type_TimeSeries, "log (TimeSeries)", f_func },
	{ FUNC_VIRTUAL, &Type_TimeSeries, "xlog (TimeSeries)", f_func },
	{ FUNC_VIRTUAL, &Type_TimeSeries, "call (VirFunc, TimeSeries)", f_xfunc },

/*	Binäre Terme
*/
	{ FUNC_VIRTUAL, &Type_TimeSeries, "operator+ (TimeSeries, TimeSeries)", f_term },
	{ FUNC_VIRTUAL, &Type_TimeSeries, "operator+ (double, TimeSeries)", f_term },
	{ FUNC_VIRTUAL, &Type_TimeSeries, "operator+ (TimeSeries, double)", f_term },
	{ FUNC_VIRTUAL, &Type_TimeSeries, "operator- (TimeSeries, TimeSeries)", f_term },
	{ FUNC_VIRTUAL, &Type_TimeSeries, "operator- (double, TimeSeries)", f_term },
	{ FUNC_VIRTUAL, &Type_TimeSeries, "operator- (TimeSeries, double)", f_term },
	{ FUNC_VIRTUAL, &Type_TimeSeries, "operator* (TimeSeries, TimeSeries)", f_term },
	{ FUNC_VIRTUAL, &Type_TimeSeries, "operator* (double, TimeSeries)", f_term },
	{ FUNC_VIRTUAL, &Type_TimeSeries, "operator* (TimeSeries, double)", f_term },
	{ FUNC_VIRTUAL, &Type_TimeSeries, "operator/ (TimeSeries, TimeSeries)", f_term },
	{ FUNC_VIRTUAL, &Type_TimeSeries, "operator/ (TimeSeries, double)", f_term },
	{ FUNC_VIRTUAL, &Type_TimeSeries, "operator/ (double, TimeSeries)", f_term },

	{ FUNC_VIRTUAL, &Type_TimeSeries, "call (VirFunc, TimeSeries, TimeSeries)", f_xterm },
	{ FUNC_VIRTUAL, &Type_TimeSeries, "call (VirFunc, TimeSeries, double)", f_xterm },
	{ FUNC_VIRTUAL, &Type_TimeSeries, "call (VirFunc, double, TimeSeries)", f_xterm },
};


void CmdSetup_TimeSeries (void)
{
	AddType(&Type_TimeSeries);
	AddFuncDef(ts_func, tabsize(ts_func));
	AddVar(Type_TimeSeries.vtab, var_TimeSeries, tabsize(var_TimeSeries));
}
