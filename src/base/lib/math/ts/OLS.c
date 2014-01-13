/*
:*:calculate OLS regression
:de:OLS Regression durchführen

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
#include <Math/linalg.h>
#include <math.h>

typedef struct {
	char *name;
	double *data;
	TimeSeries *ts;
	TimeIndex base;
	int dim;
} Exogen;

static double DurbinWatson (double *x, size_t nr)
{
	double dw, s;
	int i;

	dw = s = 0.;

	for (i = 0; i < nr; i++)
		s += x[i] * x[i];

	if	(s == 0.)	return 2.;

	for (i = 1; i < nr; i++)
	{
		register double z = x[i] - x[i-1];
		dw += z * z;
	}

	return dw / s;
}

static void Exogen_set(Exogen *ext, EfiType *type, void *data)
{
	if	(type == &Type_TimeSeries)
	{
		TimeSeries *ts = Val_TimeSeries(data);

		if	(ts != NULL)
		{
			ext->ts = ts;
			ext->name = ts->name;
			ext->data = ts->data;
			ext->base = ts->base;
			ext->dim = ts->dim;
			return;
		}
	}

	ext->ts = NULL;
	ext->name = NULL;
	ext->data = NULL;
	ext->base = tindex_create(TS_INDEX, 0);
	ext->dim = 0;
}

static void Exogen_sync (Exogen *ext, int value)
{
	int n = value - (int) ext->base.value;

	ext->base.value = value;

	if	(ext->data && n < ext->dim)
	{
		ext->data += n;
		ext->dim -= n;
	}
	else
	{
		ext->data = NULL;
		ext->dim = 0;
	}
}

static int Exogen_create (EfiObjList *list, Exogen **ptr)
{
	int dim;
	int i;

	dim = ObjListLen(list);

	if	(dim == 0)
	{
		*ptr = NULL;
		return 0;
	}

	*ptr = memalloc(dim * sizeof(Exogen));

	for (i = 0; list != NULL; list = list->next, i++)
	{
		EfiObj *obj;

		obj = EvalObj(RefObj(list->obj), NULL);

		if	(obj)
		{
			Exogen_set(*ptr + i, obj->type, obj->data);
			UnrefObj(obj);
		}
		else	Exogen_set(*ptr + i, NULL, NULL);

		if	((*ptr)[i].name == NULL)
			(*ptr)[i].name = msprintf("x%d", i + 1);
	}

	return dim;
}

static double *get_data(TimeSeries *ts, unsigned base)
{
	return ts->data + (base - ts->base.value);
}

void Func_OLS (EfiFunc *func, void *rval, void **arg)
{
	OLSPar *par;
	TimeIndex base, first, last;
	Exogen y;
	Exogen *ext;
	double *z, *p, *yb, *res;
	double ym, ys, x, sigma;
	int i, j, k;
	int nr, df, dim;

	Val_OLSPar(rval) = NULL;
	Exogen_set(&y, func->arg[0].type, arg[0]);
	dim = Exogen_create(Val_list(arg[1]), &ext);

	if	(dim == 0)
	{
		log_note(NULL, "[TimeSeries:31]", NULL);
		memfree(ext);
		return;
	}

/*	Zeitreihen synchronisieren
*/
	base = y.base;
	first = ext[0].base;
	last = ext[0].base;
	last.value += ext[0].dim - 1;

	for (i = 0; i < dim; i++)
	{
		if	(ext[i].base.type != base.type)
		{
			log_note(NULL, "[TimeSeries:32]", NULL);
			memfree(ext);
			return;
		}

		if	(base.value < ext[i].base.value)
			base.value = ext[i].base.value;

		if	(first.value < ext[i].base.value)
			first.value = ext[i].base.value;

		if	(last.value > ext[i].base.value + ext[i].dim - 1)
			last.value = ext[i].base.value + ext[i].dim - 1;
	}

	Exogen_sync(&y, base.value);
	nr = y.dim;

	for (i = 0; i < dim; i++)
	{
		Exogen_sync(ext + i, base.value);

		if	(nr > ext[i].dim)
			nr = ext[i].dim;
	}

	if	(nr < dim)
	{
		log_note(NULL, "[TimeSeries:33]", NULL);
		memfree(ext);
		return;
	}

	df = nr - dim;

/*	Matrix setzen und invertieren
*/
	z = memalloc(dim * dim * sizeof(double));
	p = z;

	for (i = 0; i < dim; i++)
	{
		for (j = 0; j < dim; j++)
		{
			*p = 0;

			for (k = 0; k < nr; k++)
				*p += ext[i].data[k] * ext[j].data[k];

			p++;
		}
	}

	if	(GaussJordan(z, dim) != dim)
	{
		log_note(NULL, "[TimeSeries:34]", NULL);
		memfree(ext);
		memfree(z);
		return;
	}

/*	Koeffizienten berechnen
*/
	par = OLSPar_create(dim);
	par->nr = nr;
	par->df = df;
	par->base = base;
	par->first = first;
	par->last = last;

	yb = memalloc(dim * sizeof(double));

	for (i = 0; i < dim; i++)
	{
		yb[i] = 0.;

		for (k = 0; k < nr; k++)
			yb[i] += ext[i].data[k] * y.data[k];
	}

	p = z;

	for (i = 0; i < dim; i++)
	{
		par->koef[i].name = mstrcpy(ext[i].name);
		par->exogen[i] = rd_refer(ext[i].ts);
		x = 0.;

		for (j = 0; j < dim; j++)
			x += p[j] * yb[j];

		par->koef[i].val = x;
		p += dim;
	}

/*	Mittelwert und Streuungsquadratsumme
*/
	ym = 0.;
	ys = 0.;

	for (i = 0; i < nr; i++)
		ym += y.data[i];

	ym /= (double) nr;

	for (i = 0; i < nr; i++)
	{
		x = (y.data[i] - ym);
		ys += x * x;
	}

/*	Residuen und Streuungsquadratsumme
*/
	par->res = ts_create(y.name, first, last.value - first.value + 1);
	res = get_data(par->res, base.value);
	sigma = 0.;

	for (i = 0; i < nr; i++)
	{
		x = y.data[i];

		for (j = 0; j < dim; j++)
			x -= par->koef[j].val * ext[j].data[i];

		res[i] = x;
		sigma += x * x;
	}

	if	(df == 0)
	{
		par->r2 = 1.;
		par->rbar2 = 1.;
		sigma = 0.;
	}
	else if	(ys)
	{
		par->r2 = 1. - sigma / ys;
		sigma /= (double) df;
		par->rbar2 = 1. - (nr - 1.) * sigma / ys;
	}
	else	par->r2 = par->rbar2 = 0.;

	par->see = sqrt(sigma);
	par->dw = DurbinWatson(res, nr);

	for (i = 0; i < dim; i++)
		par->koef[i].se = sqrt(sigma * z[i * dim + i]);

/*	Aufräumen
*/
	memfree(z);
	memfree(yb);
	memfree(ext);
	Val_OLSPar(rval) = par;
}


void Func_OLSProj (EfiFunc *func, void *rval, void **arg)
{
	TimeSeries *ts;
	TimeIndex idx;
	OLSPar *par;
	double **data, *res;
	int i, j, nr;

	Val_TimeSeries(rval) = NULL;

	if	((par = Val_OLSPar(arg[0])) == NULL)
		return;

	idx = func->dim > 1 ? Val_TimeIndex(arg[1]) : par->first;
	i = nr = tindex_diff(idx, par->last) + 1;

	if	(func->dim > 2)
	{
		if	(func->arg[2].type == &Type_TimeIndex)
			i = tindex_diff(idx, Val_TimeIndex(arg[2])) + 1;
		else if	(func->arg[2].type == &Type_int)
			i = Val_int(arg[2]);

		if	(nr > i)	nr = i;
	}

/*	Zeitreihenpointer
*/
	data = memalloc(par->dim * sizeof(double *));

	for (j = 0; j < par->dim; j++)
		data[j] = get_data(par->exogen[j], idx.value);

	res = get_data(par->res, idx.value);
	ts = ts_create(NULL, idx, nr);

	for (i = 0; i < nr; i++)
	{
		register double z = res[i];

		for (j = 0; j < par->dim; j++)
			z += par->koef[j].val * data[j][i];

		ts->data[i] = z;
	}

	Val_TimeSeries(rval) = ts;
	memfree(data);
}
