/*
:*:calculate differences
:de:Differenzen berechnen

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
#include <Math/TimeSeries.h>

#define	SetData(func, data, a, b, c)	\
	CallFunc(&Type_double, (data) + (a), func, (data) + (b), (data) + (c))

TimeSeries *ts_diff (TimeSeries *ts, int n, EfiVirFunc *fptr)
{
	EfiFunc *func;
	TimeSeries *base;
	int i;

	if	(ts == NULL)	return NULL;
	if	(n == 0)	return NULL;

	func = GetFunc(&Type_double, fptr, 2, &Type_double, 0, &Type_double, 0);

	if	(func == NULL)
	{
		log_note(NULL, "[TimeSeries:21]", NULL); 
		return NULL;
	}

	if	(n > 0)
	{
		if	(n > ts->dim)	n = ts->dim;

		base = ts_create(ts->name, ts->base, n);
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

		base = ts_create(ts->name, ts->base, n);
		base->base.value += ts->dim - n;
		ts->dim -= n;

		for (i = 0; i < n; i++)
			base->data[i] = ts->data[ts->dim + i];

		for (i = 0; i < ts->dim; i++)
			SetData(func, ts->data, i, i, i + n);
	}

	return base;
}


void ts_cumulate (TimeSeries *ts, TimeSeries *base, EfiVirFunc *fptr)
{
	EfiFunc *func;
	TimeIndex idx;
	int i, n;
	
	if	(ts->base.type != base->base.type)
	{
		log_note(NULL, "[TimeSeries:1]",
			"cc", ts->base.type, base->base.type);
		return;
	}

	func = GetFunc(&Type_double, fptr, 2, &Type_double, 0, &Type_double, 0);

	if	(func == NULL)
	{
		log_note(NULL, "[TimeSeries:21]", NULL); 
		return;
	}

	idx = ts->base;

	if	(base->base.value < ts->base.value)
		idx.value = base->base.value;

	n = ts->base.value + ts->dim;

	if	(base->base.value + base->dim > n)
		n = base->base.value + base->dim;

	n -= idx.value;
	ts_sync(ts, idx, n, 0);

/*	Basis übernehmen
*/
	n = base->base.value - idx.value;

	for (i = 0; i < base->dim; i++)
		ts->data[i + n] = base->data[i];

/*	Werte zurückrechnen
*/
	for (i = n - 1; i >= 0; i--)
		SetData(func, ts->data, i, i, i + base->dim);

/*	Werte vorrausrechnen
*/
	n = base->base.value + base->dim - idx.value;

	for (i = n; i < ts->dim; i++)
		SetData(func, ts->data, i, i, i - base->dim);
}
