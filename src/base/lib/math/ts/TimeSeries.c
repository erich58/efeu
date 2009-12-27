/*
:de:reference type for time series
:de:Referenztype der Zeitreihenstruktur

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

#define	TS_BLKSIZE	32

static ALLOCTAB(ts_tab, 0, sizeof(TimeSeries));

static void ts_clean (void *data)
{
	TimeSeries *tg = data;
	lfree(tg->data);
	memfree(tg->name);
	memfree(tg->fmt);
	del_data(&ts_tab, tg);
}

static void ts_alloc (TimeSeries *ts, size_t dim)
{
	ts->size = TS_BLKSIZE * ((dim + TS_BLKSIZE - 1) / TS_BLKSIZE);
	ts->data = lmalloc(ts->size * sizeof(double));
	memset(ts->data, 0, ts->size * sizeof(double));
}

static char *ts_ident (const void *data)
{
	const TimeSeries *ts = data;
	StrBuf *buf = sb_acquire();
	IO *io = io_strbuf(buf);

	io_xprintf(io, "%s[%d] ", ts->name, ts->dim);
	tindex_print(io, ts->base, 0);

	if	(ts->dim)
	{
		io_putc(' ', io);
		tindex_print(io, ts->base, ts->dim - 1);
	}

	io_close(io);
	return sb_cpyrelease(buf);
}


RefType TimeSeries_reftype = REFTYPE_INIT("TimeSeries", ts_ident, ts_clean);


/*	Zeitreihe generieren
*/

TimeSeries *ts_create (const char *name, TimeIndex idx, size_t dim)
{
	TimeSeries *ts = new_data(&ts_tab);
	ts->name = mstrcpy(name);
	ts->base = idx;
	ts->dim = dim;
	ts_alloc(ts, dim);
	return rd_init(&TimeSeries_reftype, ts);
}


void ts_expand(TimeSeries *ts, size_t dim)
{
	if	(ts->size < dim)
	{
		double *save;
		int i;

		save = ts->data;
		ts_alloc(ts, dim);

		for (i = 0; i < ts->dim; i++)
			ts->data[i] = save[i];

		lfree(save);
	}

	while (ts->dim < dim)
		ts->data[ts->dim++] = 0.;
}


void ts_sync (TimeSeries *ts, TimeIndex base, size_t dim, int offset)
{
	int i, k;
	unsigned tsend, end;
	double *save;

	if	(ts->base.type != base.type)
	{
		dbg_note(NULL, "[TimeSeries:1]",
			"cc", ts->base.type, base.type);
		return;
	}

	tsend = ts->base.value + ts->dim;

	if	(dim == 0)
		dim = (tsend > base.value) ? tsend - base.value : 0;

	end = base.value + dim;

/*	Zeitreihe oben kürzen
*/
	if	(end < ts->base.value)
		ts->dim = 0;
	else if	(end < tsend)
		ts->dim = end - ts->base.value;

/*	Zeitreihe unten kürzen
*/
	if	(tsend < base.value)
	{
		ts->base.value = base.value;
		ts->dim = 0;
	}
	else if	(ts->base.value < base.value)
	{
		k = base.value - ts->base.value;
		ts->dim -= k;
		ts->base.value += k;

		for (i = 0; i < ts->dim; i++)
			ts->data[i] = ts->data[i + k];
	}

/*	Zeitreihe erweitern
*/
	k = base.value < ts->base.value ? ts->base.value - base.value : 0;

	if	(ts->size < dim)
	{
		save = ts->data;
		ts_alloc(ts, dim);

		for (i = 0; i < ts->dim; i++)
			ts->data[i + k] = save[i];

		lfree(save);
	}
	else
	{
		for (i = ts->dim - 1; i >= 0; i--)
			ts->data[i + k] = ts->data[i];

		for (i = 0; i < k; i++)
			ts->data[i] = 0.;

		for (i = ts->dim + k; i < ts->size; i++)
			ts->data[i] = 0.;
	}

/*	Werte fortschreiben
*/
	if	(offset > 0)
	{
		if	(k && offset < ts->dim + k)
		{
			i = (offset > ts->dim) ? (ts->dim + k) - offset : k;

			while (i-- > 0)
				ts->data[i] = ts->data[i + offset];
		}

		i = ts->dim + k;

		if	(i < offset)
			i = offset;

		for (; i < dim; i++)
			ts->data[i] = ts->data[i - offset];
	}

	ts->base.value = base.value;
	ts->dim = dim;
}
