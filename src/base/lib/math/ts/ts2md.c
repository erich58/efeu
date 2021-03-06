/*
:*:convert time series into data cube
:de:Zeitreihe in Datenmatrix konvertieren

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

#include <Math/TimeSeries.h>
#include <EFEU/LangDef.h>

static char *type_name(int type)
{
	if	(mstrcmp(LangDef.language, "de") == 0)
	{
		switch (type)
		{
		case TS_YEAR:	return "jahr";
		case TS_QUART:	return "quart";
		case TS_MONTH:	return "monat";
		case TS_WEEK:	return "woche";
		case TS_DAY:	return "tag";
		default:	return "index";
		}
	}

	switch (type)
	{
	case TS_YEAR:	return "year";
	case TS_QUART:	return "quart";
	case TS_MONTH:	return "month";
	case TS_WEEK:	return "week";
	case TS_DAY:	return "day";
	default:	return "index";
	}
}

mdmat *TimeSeries2mdmat (TimeSeries *ts)
{
	mdmat *md;
	double *ptr;
	int i;

	if	(ts == NULL)	return NULL;

/*	Datenmatrix generieren
*/
	md = new_mdmat();
	md->sbuf = NewStrPool();
	md->type = &Type_double;
	md->i_name = StrPool_xadd(md->sbuf, ts->name);

/*	Achse initialisieren
*/
	md->axis = new_axis(md->sbuf, ts->dim);
	md->axis->i_name = StrPool_xadd(md->sbuf, type_name(ts->base.type));

	for (i = 0; i < ts->dim; i++)
	{
		char *p = TimeIndex2str(ts->base, i);
		md->axis->idx[i].i_name = StrPool_add(md->sbuf, p);
		memfree(p);
	}

/*	Datenwerte speichern
*/
	md_alloc(md);
	ptr = md->data;

	for (i = 0; i < ts->dim; i++)
		ptr[i] = ts->data[i];

	return md;
}
