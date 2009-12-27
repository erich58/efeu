/*	Implementation des Zufallszahlengenerator
	(c) 1994 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6
*/


#include <EFEU/object.h>
#include <EFEU/efutil.h>
#include <EFEU/cmdconfig.h>
#include <EFEU/rand48.h>


Type_t Type_Rand48 = REF_TYPE("Rand48", Rand48_t *);

#define	Val_Rand48(data)	((Rand48_t **) data)[0]

#define	RVRAND	Val_Rand48(rval)


CEXPR(f_newrand, RVRAND = NewRand48(Val_int(arg[0])))

static void f_copy(Func_t *func, void *rval, void **arg)
{
	RVRAND = NewRand48(0);
	RVRAND->param[0] = Val_Rand48(arg[0])->param[0];
	RVRAND->param[1] = Val_Rand48(arg[0])->param[1];
	RVRAND->param[2] = Val_Rand48(arg[0])->param[2];
}

CEXPR(f_seed, SeedRand48(Val_Rand48(arg[0]), Val_int(arg[1])))
CEXPR(f_drand, Val_double(rval) = Rand48(Val_Rand48(arg[0])))
CEXPR(f_nrand, Val_double(rval) = NormalRand48(Val_Rand48(arg[0]), 0))

static void f_rdround(Func_t *func, void *rval, void **arg)
{
	Val_int(rval) = Rand48Round(Val_Rand48(arg[0]),
		Val_double(arg[1]));
}

static Var_t vdef[] = {
	{ "Rand48Debug", &Type_bool, &Rand48_reftype.debug },
};

static FuncDef_t fdef[] = {
	{ 0, &Type_Rand48, "Rand48 (int seed = 0)", f_newrand },
	{ 0, &Type_Rand48, "Rand48::copy ()", f_copy },
	{ 0, &Type_void, "Rand48::seed (int value)", f_seed },
	{ 0, &Type_double, "Rand48::drand ()", f_drand },
	{ 0, &Type_double, "Rand48::nrand ()", f_nrand },
	{ 0, &Type_int, "Rand48::round (double)", f_rdround },

	{ 0, &Type_void, "srand48 (Rand48 rand, int value = 0)", f_seed },
	{ 0, &Type_double, "drand48 (Rand48 rand = NULL)", f_drand },
	{ 0, &Type_double, "nrand48 (Rand48 rand = NULL)", f_nrand },
};


/*	Globale Variablen
*/

void SetupRand48()
{
	AddType(&Type_Rand48);
	AddVar(NULL, vdef, tabsize(vdef));
	AddFuncDef(fdef, tabsize(fdef));
}
