/*
:*:count parameterss
:de:Zählparameter

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

#include <EFEU/mdcount.h>
#include <EFEU/parsearg.h>

#define	VBSIZE	60
#define	CBSIZE	1000

static char *rt_ident (const void *ptr)
{
	const MdCountPar *par = ptr;
	return msprintf("%s[%d, %d, %d]", par->reftype->label,
		par->vtab.used, par->ctab.used, par->time_dim);
}

static void rt_clean (void *ptr)
{
	MdCountPar *par = ptr;
	vb_clean(&par->vtab, NULL);
	vb_clean(&par->ctab, NULL);
	memfree(par->time_range);
	del_axis(par->axis);
	rd_deref(par->cpar);
	UnrefObj(par->obj);
	rd_deref(par->md);
	rd_deref(par->edb);
	memfree(par);
}

static RefType reftype = REFTYPE_INIT("MdCountPar", rt_ident, rt_clean);

static int rec_count (MdCountPar *par, TimeRange *range)
{
	MdCountPar_event(par, range ? range->dat : 0, 1.);
	return 1;
}

static double wcount_value = 0.;

static void add_wcount (MdCount *cnt, void *data, void *buf)
{
	Val_double(data) += wcount_value;
}

static MdCount ctab[] = {
	{ "wcount", "double", "weighted count", NULL, NULL, add_wcount },
};

MdCountPar* MdCountPar_create (void)
{
	MdCountPar *par = memalloc(sizeof *par);
	par->pool = NewStrPool();
	vb_init(&par->vtab, VBSIZE, sizeof(MdCount *));
	vb_init(&par->ctab, CBSIZE, sizeof(MdClass *));
	MdCount_add(par, stdcount, 1);
	MdCount_add(par, ctab, tabsize(ctab));
	par->time_range = NULL;
	par->time_dim = 0;
	par->axis = NULL;
	par->cpar = NULL;
	par->md = NULL;
	par->edb = NULL;
	par->obj = NULL;
	par->count = rec_count;
	par->recnum = 0;
	par->event = 0;
	par->weight = 1;
	return rd_init(&reftype, par);
}


int MdCountPar_count (MdCountPar *par)
{
	par->recnum++;
	par->event = 0;

	if	(par->time_dim)
	{
		int n = 0;

		for (par->time_idx = 0; par->time_idx < par->time_dim;
			par->time_idx++)
		{
			n += par->count(par, par->time_range + par->time_idx);
		}

		return n;
	}
	else	return par->count(par, NULL);
}

void MdCountPar_event (MdCountPar *par, unsigned dat, double weight)
{
	par->event++;
	wcount_value = par->weight * weight; 
	md_count(par->md, NULL);
}
