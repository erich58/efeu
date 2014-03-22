/*
Polynom - Funktionen

$Copyright (C) 1994 Erich Frühstück
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
#include <Math/pnom.h>


EfiType Type_pnom = REF_TYPE("Polynom", Polynom *);

Polynom *Buf_pnom = NULL;

#define	REF(ptr)	rd_refer(Val_pnom(ptr))


/*	Funktionen
*/

static void PF_create(EfiFunc *func, void *rval, void **arg)
{
	Polynom *pn;
	EfiObjList *list;
	int i;

	Val_pnom(rval) = pnconst(0., Val_double(arg[0]));
	list = Val_list(arg[0]);

	pn = pnalloc(1, ObjListLen(list) - 1);
	pn->x[0] = 0.;

	for (i = 0; list != NULL; list = list->next)
		pn->c[0][i++] = Obj2double(RefObj(list->obj));

	Val_pnom(rval) = pn;
}


static void PF_load(EfiFunc *func, void *rval, void **arg)
{
	Val_pnom(rval) = pnload(Val_str(arg[0]));
}

static void PF_save(EfiFunc *func, void *rval, void **arg)
{
	register Polynom *pn = Val_pnom(arg[0]);
	pnsave(pn, Val_str(arg[1]));
	Val_pnom(rval) = pn;
}

static void PF_print (EfiFunc *func, void *rval, void **arg)
{
	Val_int(rval) = pnprint(Val_io(arg[1]), Val_pnom(arg[0]),
		Val_str(arg[2]));
}

static size_t make_xy (void *p1, void *p2, double **x, double **y)
{
	EfiVec *v1, *v2;
	size_t i, dim;

	v1 = Val_ptr(p1);
	v2 = Val_ptr(p2);
	dim = (v1 && v2) ? min(v1->buf.used, v2->buf.used) : 0;

	if	(dim == 0)	return 0;

	*x = memalloc(dim * sizeof(double));
	*y = memalloc(dim * sizeof(double));

	for (i = 0; i < dim; i++)
	{
		(*x)[i] = Obj2double(Vector(v1, i));
		(*y)[i] = Obj2double(Vector(v2, i));
	}

	return dim;
}

static void PF_linint (EfiFunc *func, void *rval, void **arg)
{
	size_t dim;
	double *x, *y;

	dim = make_xy(arg[0], arg[1], &x, &y);
	Val_pnom(rval) = linint(dim, x, y);
	memfree(x);
	memfree(y);
}


static void PF_spline (EfiFunc *func, void *rval, void **arg)
{
	size_t dim;
	double *x, *y;

	dim = make_xy(arg[0], arg[1], &x, &y);
	Val_pnom(rval) = spline(dim, x, y);
	memfree(x);
	memfree(y);
}


static void PF_dspline (EfiFunc *func, void *rval, void **arg)
{
	size_t dim;
	double *x, *y;

	dim = make_xy(arg[0], arg[1], &x, &y);
	Val_pnom(rval) = dspline(dim, x, y, Val_double(arg[2]),
		Val_double(arg[3]));
	memfree(x);
	memfree(y);
}

static void PF_eval(EfiFunc *func, void *rval, void **arg)
{
	EfiObjList *list;
	double x;
	int deg;

	list = Val_list(arg[1]);
	x = 0.;
	deg = 0;

	if	(list)
	{
		x = Obj2double(RefObj(list->obj));

		if	(list->next)
			deg = Obj2int(RefObj(list->next->obj));
	}

	Val_double(rval) = pneval(Val_pnom(arg[0]), deg, x);
}


static EfiFuncDef fdef[] = {
	{ 0, &Type_pnom, "linint (EfiVec x, EfiVec y)", PF_linint },
	{ 0, &Type_pnom, "spline (EfiVec x, EfiVec y)", PF_spline },
	{ 0, &Type_pnom, "dspline (EfiVec x, EfiVec y, double a = 0., double b = 0.)", PF_dspline },
	{ 0, &Type_pnom, "Polynom (...)", PF_create },
	{ 0, &Type_pnom, "pnload (str file)", PF_load },
	{ 0, &Type_pnom, "Polynom::save (str file)", PF_save },
	{ FUNC_VIRTUAL, &Type_double, "operator() (Polynom, List_t)", PF_eval },
	{ 0, &Type_int, "Polynom::print(IO = iostd, str fmt = \"%g\")",
		PF_print },
};


void SetupPnom (void)
{
	static int init_done = 0;

	if	(init_done)	return;

	init_done = 1;

	AddType(&Type_pnom);
	AddFuncDef(fdef, tabsize(fdef));
}
