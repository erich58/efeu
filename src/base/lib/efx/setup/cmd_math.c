/*
Mathematische Funktionen

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
#include <EFEU/cmdconfig.h>
#include <math.h>

/*	Funktionsmakros
*/

#define	DOUBLE(n)	Val_double(arg[n])
#define	DRETVAL		Val_double(rval)

#define	EXPR(name, expr)	\
CEXPR(name, DRETVAL = expr)


/*	Funktionen
*/

EXPR(f_sqrt, sqrt(DOUBLE(0)));
EXPR(f_log, log(DOUBLE(0)));
EXPR(f_xlog, DOUBLE(0) > 0. ? log(DOUBLE(0)) : 0.);
EXPR(f_exp, exp(DOUBLE(0)));
EXPR(f_ceil, ceil(DOUBLE(0)));
EXPR(f_floor, floor(DOUBLE(0)));
EXPR(f_sin, sin(DOUBLE(0)));
EXPR(f_cos, cos(DOUBLE(0)));
EXPR(f_tan, tan(DOUBLE(0)));
EXPR(f_asin, asin(DOUBLE(0)));
EXPR(f_acos, acos(DOUBLE(0)));
EXPR(f_atan, atan(DOUBLE(0)));
EXPR(f_atan2, atan2(DOUBLE(0), DOUBLE(1)));
EXPR(f_pow, pow(DOUBLE(0), DOUBLE(1)));
EXPR(f_fmod, fmod(DOUBLE(0), DOUBLE(1)));

static FuncDef_t fdef_math[] = {
	{ FUNC_VIRTUAL, &Type_double, "log (double)", f_log },
	{ FUNC_VIRTUAL, &Type_double, "xlog (double)", f_xlog },
	{ FUNC_VIRTUAL, &Type_double, "exp (double)", f_exp },
	{ FUNC_VIRTUAL, &Type_double, "pow (double, double)", f_pow },
	{ FUNC_VIRTUAL, &Type_double, "ceil (double)", f_ceil },
	{ FUNC_VIRTUAL, &Type_double, "floor (double)", f_floor },
	{ FUNC_VIRTUAL, &Type_double, "sin (double)", f_sin },
	{ FUNC_VIRTUAL, &Type_double, "cos (double)", f_cos },
	{ FUNC_VIRTUAL, &Type_double, "tan (double)", f_tan },
	{ FUNC_VIRTUAL, &Type_double, "asin (double)", f_asin },
	{ FUNC_VIRTUAL, &Type_double, "acos (double)", f_acos },
	{ FUNC_VIRTUAL, &Type_double, "atan (double)", f_atan },
	{ FUNC_VIRTUAL, &Type_double, "atan2 (double, double)", f_atan2 },
	{ FUNC_VIRTUAL, &Type_double, "sqrt (double)", f_sqrt },
	{ FUNC_VIRTUAL, &Type_double, "operator% (double, double)", f_fmod },
};


/*	Initialisierung
*/

void SetupMath(void)
{
	AddFuncDef(fdef_math, tabsize(fdef_math));
}
