/*	Regressionskoeffizienten
	(c) 1997 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6

	Version 1.0
*/

#include <EFEU/object.h>
#include <EFEU/stdtype.h>
#include <Math/TimeSeries.h>


/*	Datentype
*/

Type_t Type_OLSPar = REF_TYPE("OLSPar", OLSPar_t *);

/*	Komponentenfunktionen
*/

static int *par_nr (OLSPar_t **par)
{
	return *par ? &(*par)->nr : NULL;
}

static int *par_df (OLSPar_t **par)
{
	return *par ? &(*par)->df : NULL;
}

static double *par_r2 (OLSPar_t **par)
{
	return *par ? &(*par)->r2 : NULL;
}

static double *par_rbar2 (OLSPar_t **par)
{
	return *par ? &(*par)->rbar2 : NULL;
}

static double *par_dw (OLSPar_t **par)
{
	return *par ? &(*par)->dw : NULL;
}

static double *par_see (OLSPar_t **par)
{
	return *par ? &(*par)->see : NULL;
}

static TimeIndex_t *par_base (OLSPar_t **par)
{
	return *par ? &(*par)->base : NULL;
}

static TimeIndex_t *par_first (OLSPar_t **par)
{
	return *par ? &(*par)->first : NULL;
}

static TimeIndex_t *par_last (OLSPar_t **par)
{
	return *par ? &(*par)->last : NULL;
}


static Obj_t *par_koef (const Var_t *st, const Obj_t *obj)
{
	OLSPar_t *x;
	
	x = (obj ? Val_OLSPar(obj->data) : NULL);

	if	(x == NULL)	return NULL;

	Buf_vec.type = &Type_OLSKoef;
	Buf_vec.dim = x->dim;
	Buf_vec.data = x->koef;
	return NewObj(&Type_vec, &Buf_vec);
}

static Obj_t *par_exogen (const Var_t *st, const Obj_t *obj)
{
	OLSPar_t *x;
	
	x = (obj ? Val_OLSPar(obj->data) : NULL);

	if	(x == NULL)	return NULL;

	Buf_vec.type = &Type_TimeSeries;
	Buf_vec.dim = x->dim;
	Buf_vec.data = x->exogen;
	return NewObj(&Type_vec, &Buf_vec);
}


static Obj_t *par_res (const Var_t *st, const Obj_t *obj)
{
	OLSPar_t *x = (obj ? Val_OLSPar(obj->data) : NULL);
	return x ? NewPtrObj(&Type_TimeSeries, rd_refer(x->res)) : NULL;
}

static Var_t var_OLSPar[] = {
	{ "nr", &Type_int, NULL, 0, 0, ConstMember, par_nr },
	{ "df", &Type_int, NULL, 0, 0, ConstMember, par_df },
	{ "r2", &Type_double, NULL, 0, 0, ConstMember, par_r2 },
	{ "rbar2", &Type_double, NULL, 0, 0, ConstMember, par_rbar2 },
	{ "dw", &Type_double, NULL, 0, 0, ConstMember, par_dw },
	{ "see", &Type_double, NULL, 0, 0, ConstMember, par_see },
	{ "koef", &Type_vec, NULL, 0, 0, par_koef, NULL },
	{ "exogen", &Type_vec, NULL, 0, 0, par_exogen, NULL },
	{ "res", &Type_TimeSeries, NULL, 0, 0, par_res, NULL },
	{ "base", &Type_TimeIndex, NULL, 0, 0, ConstMember, par_base },
	{ "first", &Type_TimeIndex, NULL, 0, 0, ConstMember, par_first },
	{ "last", &Type_TimeIndex, NULL, 0, 0, ConstMember, par_last },
};

/*	Funktionen
*/

static void f_fprint (Func_t *func, void *rval, void **arg)
{
	io_t *io = Val_io(arg[0]);
	OLSPar_t *par = Val_OLSPar(arg[1]);
	Val_int(rval) = par ? PrintOLSPar(io, par) : io_puts("NULL", io);
}


/*	Initialisieren
*/

static FuncDef_t func[] = {
	{ FUNC_VIRTUAL, &Type_OLSPar, "OLS (TimeSeries y, ...)",
		Func_OLS },
	{ FUNC_VIRTUAL, &Type_TimeSeries,
		"OLSPar::proj ()", Func_OLSProj },
	{ FUNC_VIRTUAL, &Type_TimeSeries,
		"OLSPar::proj (TimeIndex)", Func_OLSProj },
	{ FUNC_VIRTUAL, &Type_TimeSeries,
		"OLSPar::proj (TimeIndex, TimeIndex)", Func_OLSProj },
	{ FUNC_VIRTUAL, &Type_TimeSeries,
		"OLSPar::proj (TimeIndex, int n)", Func_OLSProj },
	{ FUNC_VIRTUAL, &Type_int, "fprint (IO, OLSPar)", f_fprint },
};


void CmdSetup_OLSPar (void)
{
	AddType(&Type_OLSPar);
	AddFuncDef(func, tabsize(func));
	AddVar(Type_OLSPar.vtab, var_OLSPar, tabsize(var_OLSPar));
}
