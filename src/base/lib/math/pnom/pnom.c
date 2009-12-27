/*	Polynom - Funktionen
	(c) 1994 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6

	Version 3.0
*/

#include <EFEU/object.h>
#include <EFEU/stdtype.h>
#include <Math/pnom.h>


Type_t Type_pnom = REF_TYPE("Polynom", pnom_t *);

pnom_t *Buf_pnom = NULL;

#define	REF(ptr)	rd_refer(Val_pnom(ptr))


/*	Funktionen
*/

static void PF_create(Func_t *func, void *rval, void **arg)
{
	pnom_t *pn;
	ObjList_t *list;
	int i;

	Val_pnom(rval) = pnconst(0., Val_double(arg[0]));
	list = Val_list(arg[0]);

	pn = pnalloc(1, ObjListLen(list) - 1);
	pn->x[0] = 0.;

	for (i = 0; list != NULL; list = list->next)
		pn->c[0][i++] = Obj2double(RefObj(list->obj));

	Val_pnom(rval) = pn;
}


static void PF_load(Func_t *func, void *rval, void **arg)
{
	Val_pnom(rval) = pnload(Val_str(arg[0]));
}

static void PF_save(Func_t *func, void *rval, void **arg)
{
	register pnom_t *pn = Val_pnom(arg[0]);
	pnsave(pn, Val_str(arg[1]));
	Val_pnom(rval) = pn;
}

static void PF_print (Func_t *func, void *rval, void **arg)
{
	Val_int(rval) = pnprint(Val_io(arg[1]), Val_pnom(arg[0]),
		Val_str(arg[2]));
}

static size_t make_xy(Vec_t *v1, Vec_t *v2, double **x, double **y)
{
	size_t i, dim;

	dim = min(v1->dim, v2->dim);

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

static void PF_linint (Func_t *func, void *rval, void **arg)
{
	size_t dim;
	double *x, *y;

	dim = make_xy(arg[0], arg[1], &x, &y);
	Val_pnom(rval) = linint(dim, x, y);
	memfree(x);
	memfree(y);
}


static void PF_spline (Func_t *func, void *rval, void **arg)
{
	size_t dim;
	double *x, *y;

	dim = make_xy(arg[0], arg[1], &x, &y);
	Val_pnom(rval) = spline(dim, x, y);
	memfree(x);
	memfree(y);
}


static void PF_dspline (Func_t *func, void *rval, void **arg)
{
	size_t dim;
	double *x, *y;

	dim = make_xy(arg[0], arg[1], &x, &y);
	Val_pnom(rval) = dspline(dim, x, y, Val_double(arg[2]),
		Val_double(arg[3]));
	memfree(x);
	memfree(y);
}

static void PF_eval(Func_t *func, void *rval, void **arg)
{
	ObjList_t *list;
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


static FuncDef_t fdef[] = {
	{ 0, &Type_pnom, "linint (Vec_t x, Vec_t y)", PF_linint },
	{ 0, &Type_pnom, "spline (Vec_t x, Vec_t y)", PF_spline },
	{ 0, &Type_pnom, "dspline (Vec_t x, Vec_t y, double a = 0., double b = 0.)", PF_dspline },
	{ 0, &Type_pnom, "Polynom (...)", PF_create },
	{ 0, &Type_pnom, "pnload (str file)", PF_load },
	{ 0, &Type_pnom, "Polynom::save (str file)", PF_save },
	{ FUNC_VIRTUAL, &Type_double, "operator() (Polynom, List_t)", PF_eval },
	{ 0, &Type_int, "Polynom::print(IO = iostd, str fmt = \"%g\")",
		PF_print },
};


/*	Variablen
*/

static Var_t vardef[] = {
	{ "pndebug", &Type_bool, &pnreftype.debug },
};


void SetupPnom(void)
{
	AddType(&Type_pnom);
	AddFuncDef(fdef, tabsize(fdef));
	AddVar(NULL, vardef, tabsize(vardef));
}
