/*	Nullstellenbestimmung
	(c) 1994 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6

	Version 2.0
*/

#include <EFEU/pconfig.h>
#include <Math/solve.h>


/*	Variablendefinitionen
*/

int SolveFlag = 0;
double SolveEps = 0.000001;
int SolveStep = 25;
char *SolveFmt = "%12lG";

static Var_t solve_var[] = {
	{ "debug", 	&Type_bool, &SolveFlag },
	{ "epsilon", 	&Type_double, &SolveEps },
	{ "maxstep", 	&Type_int, &SolveStep },
	{ "fmt", 	&Type_str, &SolveFmt },
};

VarTab_t *SolvePar = NULL;

static Var_t glob_var[] = {
	{ "SolvePar",	&Type_vtab, &SolvePar },
};

static int solve_intval(void *par, double x)
{
	long n;

	n = 0;
	CallFunc(&Type_long, &n, par, &x);
	return n;
}


static double solve_dblval(void *par, double x)
{
	double z;

	z = 0.;
	CallFunc(&Type_double, &z, par, &x);
	return z;
}

static void solve_bisection(Func_t *func, void *rval, void **arg)
{
	Func_t *f;

	f = GetFunc(&Type_int, Val_vfunc(arg[0]), 1, &Type_double, 0);
	Val_double(rval) = bisection(solve_intval, f,
		Val_double(arg[1]), Val_double(arg[2]));
}

static void solve_regfalsi(Func_t *func, void *rval, void **arg)
{
	Func_t *f;

	f = GetFunc(&Type_double, Val_vfunc(arg[0]), 1, &Type_double, 0);
	Val_double(rval) = regfalsi(solve_dblval, f,
		Val_double(arg[1]), Val_double(arg[2]));
}

static FuncDef_t solve_Func[] = {
	{  0, &Type_double, "bisection(VirFunc func, double a, double b)", solve_bisection },
	{ 0, &Type_double, "regfalsi(VirFunc func, double a, double b)", solve_regfalsi },
};


/*	Initialisierung
*/

void SetupSolve()
{
	SolvePar = VarTab("SolvePar", 32);
	AddVar(SolvePar, solve_var, tabsize(solve_var));
	AddVar(NULL, glob_var, tabsize(glob_var));
	AddFuncDef(solve_Func, tabsize(solve_Func));
}
