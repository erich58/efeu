/*
Zeitreihenterme berechnen

$Copyright (C) 1997 Erich Fr�hst�ck
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
#include <Math/TimeSeries.h>

void AssignTimeSeries (VirFunc_t *fptr, TimeSeries_t *t1,
	Type_t *type, void *data)
{
	Func_t *func;
	TimeSeries_t *t2;
	double *p1, *p2;
	int s2, i, n;

	if	(t1 == NULL)	return;

	p1 = t1->data;
	n = t1->dim;

/*	Rechtes Argument bestimmen
*/
	if	(type == &Type_TimeSeries)
	{
		if	((t2 = Val_TimeSeries(data)) == NULL)
		{
			errmsg(MSG_TS, 22); 
			return;
		}
		else if	(t1->base.type != t2->base.type)
		{
			errmsg(MSG_TS, 23); 
			return;
		}

		p2 = t2->data;
		s2 = 1;

		if	(t1->base.value >= t2->base.value)
		{
			n = t1->base.value - t2->base.value;
			p2 += n;
			n = (n < t2->dim) ? t2->dim - n : 0;
			n = min(n, t1->dim);
		}
		else
		{
			n = t2->base.value - t1->base.value;
			p1 += n;
			n = (n < t1->dim) ? t1->dim - n : 0;
			n = min(n, t2->dim);
		}
	}
	else if	(type != &Type_double)
	{
		return;
	}
	else
	{
		p2 = data;
		s2 = 0;
	}

	func = GetFunc(&Type_double, fptr, 2, &Type_double, 1, &Type_double, 0);

	if	(func == NULL)
	{
		errmsg(MSG_TS, 24); 
		return;
	}

	for (i = 0; i < n; i++)
	{
		CallVoidFunc(func, p1, p2);
		p1 += 1;
		p2 += s2;
	}
}


TimeSeries_t *TimeSeriesFunc (VirFunc_t *fptr, TimeSeries_t *t1)
{
	TimeSeries_t *ts;
	Func_t *func;
	int i;

	func = GetFunc(&Type_double, fptr, 1, &Type_double, 0);

	if	(func == NULL)
	{
		errmsg(MSG_TS, 21); 
		return NULL;
	}

	ts = NewTimeSeries(NULL, t1->base, t1->dim);

	for (i = 0; i < t1->dim; i++)
		CallFunc(&Type_double, ts->data + i, func, t1->data + i);

	return ts;
}


TimeSeries_t *TimeSeriesTerm (VirFunc_t *fptr,
	Type_t *type1, void *arg1, Type_t *type2, void *arg2)
{
	Func_t *func;
	TimeSeries_t *ts, *t1, *t2;
	TimeIndex_t idx;
	double *p1, *p2;
	int s1, s2, n;

/*	Linkes Argument bestimmen
*/
	if	(type1 == &Type_TimeSeries)
	{
		if	((t1 = Val_TimeSeries(arg1)) == NULL)
			return NULL;
	}
	else if	(type1 != &Type_double)
	{
		return NULL;
	}
	else	t1 = NULL;

/*	Rechtes Argument bestimmen
*/
	if	(type2 == &Type_TimeSeries)
	{
		if	((t2 = Val_TimeSeries(arg2)) == NULL)
		{
			errmsg(MSG_TS, 22); 
			return NULL;
		}
		else if	(t1 && t1->base.type != t2->base.type)
		{
			errmsg(MSG_TS, 23); 
			return NULL;
		}
	}
	else if	(type2 != &Type_double)
	{
		return NULL;
	}
	else	t2 = NULL;

	func = GetFunc(&Type_double, fptr, 2, &Type_double, 0, &Type_double, 0);

	if	(func == NULL)
	{
		errmsg(MSG_TS, 24); 
		return NULL;
	}

	if	(t1 && t2)
	{
		p1 = t1->data;
		s1 = 1;
		p2 = t2->data;
		s2 = 1;

		if	(t1->base.value >= t2->base.value)
		{
			idx = t1->base;
			n = t1->base.value - t2->base.value;
			p2 += n;
			n = (n < t2->dim) ? t2->dim - n : 0;
			n = min(n, t1->dim);
		}
		else
		{
			idx = t2->base;
			n = t2->base.value - t1->base.value;
			p1 += n;
			n = (n < t1->dim) ? t1->dim - n : 0;
			n = min(n, t2->dim);
		}
	}
	else if	(t1)
	{
		idx = t1->base;
		n = t1->dim;
		p1 = t1->data;
		s1 = 1;
		p2 = arg2;
		s2 = 0;
	}
	else if	(t2)
	{
		idx = t2->base;
		n = t2->dim;
		p1 = arg1;
		s1 = 0;
		p2 = t2->data;
		s2 = 1;
	}
	else
	{
		idx = TimeIndex(TS_INDEX, 0);
		n = 1;
		p1 = arg1;
		s1 = 0;
		p2 = arg2;
		s2 = 0;
	}

	ts = NewTimeSeries(NULL, idx, n);

	for (n = 0; n < ts->dim; n++)
	{
		CallFunc(&Type_double, ts->data + n, func, p1, p2);
		p1 += s1;
		p2 += s2;
	}

	return ts;
}
