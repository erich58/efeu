/*
Nullstellenbestimmung

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

#include <EFEU/pconfig.h>
#include <Math/solve.h>


/*	Variablendefinitionen
*/

int SolveFlag = 0;
double SolveEps = 0.000001;
int SolveStep = 25;
char *SolveFmt = "%12lG";

static EfiVarDef solve_var[] = {
	{ "debug", 	&Type_bool, &SolveFlag },
	{ "epsilon", 	&Type_double, &SolveEps },
	{ "maxstep", 	&Type_int, &SolveStep },
	{ "fmt", 	&Type_str, &SolveFmt },
};

EfiVarTab *SolvePar = NULL;

static EfiVarDef glob_var[] = {
	{ "SolvePar", &Type_vtab, &SolvePar,
		":*:solve parameter tab\n"
		":de:Lösungsparametertabelle\n" },
};

static int solve_intval(void *par, double x)
{
	int n = 0;
	CallFunc(&Type_int, &n, par, &x);
	return n;
}


static double solve_dblval(void *par, double x)
{
	double z;

	z = 0.;
	CallFunc(&Type_double, &z, par, &x);
	return z;
}

static void solve_bisection(EfiFunc *func, void *rval, void **arg)
{
	EfiFunc *f;

	f = GetFunc(&Type_int, Val_vfunc(arg[0]), 1, &Type_double, 0);
	Val_double(rval) = bisection(solve_intval, f,
		Val_double(arg[1]), Val_double(arg[2]));
}

static void solve_regfalsi(EfiFunc *func, void *rval, void **arg)
{
	EfiFunc *f;

	f = GetFunc(&Type_double, Val_vfunc(arg[0]), 1, &Type_double, 0);
	Val_double(rval) = regfalsi(solve_dblval, f,
		Val_double(arg[1]), Val_double(arg[2]));
}

static EfiFuncDef solve_Func[] = {
	{  0, &Type_double, "bisection(VirFunc func, double a, double b)",
		solve_bisection },
	{ 0, &Type_double, "regfalsi(VirFunc func, double a, double b)",
		solve_regfalsi },
};


/*	Initialisierung
*/

void SetupSolve()
{
	SolvePar = VarTab("SolvePar", 32);
	AddVarDef(SolvePar, solve_var, tabsize(solve_var));
	AddVarDef(NULL, glob_var, tabsize(glob_var));
	AddFuncDef(solve_Func, tabsize(solve_Func));
}
