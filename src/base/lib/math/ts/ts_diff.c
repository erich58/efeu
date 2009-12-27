/*	Zeitreihen kopieren
	(c) 1997 Erich Fr�hst�ck
	A-1090 Wien, W�hringer Stra�e 64/6

	Version 1.0
*/

#include <EFEU/object.h>
#include <Math/TimeSeries.h>

#define	SetData(func, data, a, b, c)	\
	CallFunc(&Type_double, (data) + (a), func, (data) + (b), (data) + (c))

TimeSeries_t *DiffTimeSeries (TimeSeries_t *ts, int n, VirFunc_t *fptr)
{
	Func_t *func;
	TimeSeries_t *base;
	int i;

	if	(ts == NULL)	return NULL;
	if	(n == 0)	return NULL;

	func = GetFunc(&Type_double, fptr, 2, &Type_double, 0, &Type_double, 0);

	if	(func == NULL)
	{
		errmsg(MSG_TS, 21); 
		return NULL;
	}

	if	(n > 0)
	{
		if	(n > ts->dim)	n = ts->dim;

		base = NewTimeSeries(ts->name, ts->base, n);
		ts->base.value += n;
		ts->dim -= n;

		for (i = 0; i < n; i++)
			base->data[i] = ts->data[i];

		for (i = 0; i < ts->dim; i++)
			SetData(func, ts->data, i, i + n, i);
	}
	else
	{
		n = -n;

		if	(n > ts->dim)	n = ts->dim;

		base = NewTimeSeries(ts->name, ts->base, n);
		base->base.value += ts->dim - n;
		ts->dim -= n;

		for (i = 0; i < n; i++)
			base->data[i] = ts->data[ts->dim + i];

		for (i = 0; i < ts->dim; i++)
			SetData(func, ts->data, i, i, i + n);
	}

	return base;
}


void CumulateTimeSeries (TimeSeries_t *ts, TimeSeries_t *base, VirFunc_t *fptr)
{
	Func_t *func;
	TimeIndex_t idx;
	int i, n;
	
	if	(ts->base.type != base->base.type)
	{
		reg_fmt(1, "%#c", ts->base.type);
		reg_fmt(2, "%#c", base->base.type);
		errmsg(MSG_TS, 1);
		return;
	}

	func = GetFunc(&Type_double, fptr, 2, &Type_double, 0, &Type_double, 0);

	if	(func == NULL)
	{
		errmsg(MSG_TS, 21); 
		return;
	}

	idx = ts->base;

	if	(base->base.value < ts->base.value)
		idx.value = base->base.value;

	n = ts->base.value + ts->dim;

	if	(base->base.value + base->dim > n)
		n = base->base.value + base->dim;

	n -= idx.value;
	SyncTimeSeries(ts, idx, n);

/*	Basis �bernehmen
*/
	n = base->base.value - idx.value;

	for (i = 0; i < base->dim; i++)
		ts->data[i + n] = base->data[i];

/*	Werte zur�ckrechnen
*/
	for (i = n - 1; i >= 0; i--)
		SetData(func, ts->data, i, i, i + base->dim);

/*	Werte vorrausrechnen
*/
	n = base->base.value + base->dim - idx.value;

	for (i = n; i < ts->dim; i++)
		SetData(func, ts->data, i, i, i - base->dim);
}
