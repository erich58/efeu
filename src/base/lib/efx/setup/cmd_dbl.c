/*
Gleitkommaobjekte

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
#include <ctype.h>

/*	Funktionsmakros
*/

#define	DOUBLE(n)	Val_double(arg[n])
#define	STR(n)		Val_str(arg[n])
#define	DRETVAL		Val_double(rval)

#define	EXPR(name, expr)	\
CEXPR(name, DRETVAL = expr)

#define	BINARY(name, op)	\
EXPR(name, DOUBLE(0) op DOUBLE(1))

#define	COMPARE(name, op)	\
CEXPR(name, Val_int(rval) = DOUBLE(0) op DOUBLE(1))

#define	ASSIGN(name, op)	\
CEXPR(name, DOUBLE(0) op DOUBLE(1))


/*	Funktionen
*/

static void f_dbl_abs (Func_t *func, void *rval, void **arg)
{
	register double a = DOUBLE(0);
	DRETVAL = a < 0 ? -a : a;
}

static void f_dbl_min (Func_t *func, void *rval, void **arg)
{
	register double a = DOUBLE(0);
	register double b = DOUBLE(1);
	DRETVAL = a < b ? a : b;
}

static void f_dbl_max (Func_t *func, void *rval, void **arg)
{
	register double a = DOUBLE(0);
	register double b = DOUBLE(1);
	DRETVAL = a > b ? a : b;
}

static void f_dbl_xdiv (Func_t *func, void *rval, void **arg)
{
	register double a = DOUBLE(0);
	register double b = DOUBLE(1);
	DRETVAL = b ? a / b : 0.;
}

static void f_dbl_xdiv1 (Func_t *func, void *rval, void **arg)
{
	register double a = DOUBLE(0);
	register double b = DOUBLE(1);
	DRETVAL = b ? a / b : 1.;
}

static void f_dbl_idx (Func_t *func, void *rval, void **arg)
{
	register double a = DOUBLE(0);
	register double b = DOUBLE(1);
	DRETVAL = b ? 100. * a / b : 0.;
}

static void f_dbl_rate (Func_t *func, void *rval, void **arg)
{
	register double a = DOUBLE(0);
	register double b = DOUBLE(1);
	DRETVAL = b ? 100. * (a - b) / b : 0.;
}

static void f_dbl_rnd (Func_t *func, void *rval, void **arg)
{
	register double a = DOUBLE(0);
	Val_int(rval) = a < 0 ? - (int) (-a + 0.5) : (int) (a + 0.5);
}

static void f_dbl_adjust (Func_t *func, void *rval, void **arg)
{
	register double a = DOUBLE(0);
	register char *b = STR(1);

	if	(b == NULL)
	{
		DRETVAL = a;
		return;
	}

	while (isspace(*b))
		b++;

	if	(*b == '+')	DRETVAL = a + atof(b + 1);
	else if	(*b == '-')	DRETVAL = a - atof(b + 1);
	else if	(isdigit(*b))	DRETVAL = atof(b);
	else			DRETVAL = a;
}

/*	Unäre Operatoren
*/

CEXPR(l_dbl_not, Val_bool(rval) = DOUBLE(0) == 0.)
EXPR(l_dbl_minus, -DOUBLE(0))
EXPR(l_dbl_plus, DOUBLE(0))

/*	Binäre Operatoren
*/

BINARY(b_dbl_mul, *)
BINARY(b_dbl_div, /)
BINARY(b_dbl_add, +)
BINARY(b_dbl_sub, -)

/*	Vergleichsoperatoren
*/

COMPARE(b_dbl_lt, <)
COMPARE(b_dbl_le, <=)
COMPARE(b_dbl_eq, ==)
COMPARE(b_dbl_ne, !=)
COMPARE(b_dbl_ge, >=)
COMPARE(b_dbl_gt, >)

/*	Zuweisungsoperatoren
*/

#if	MAKE_ASSIGN_OP
CEXPR(l_dbl_inc, ++DOUBLE(0))
CEXPR(l_dbl_dec, --DOUBLE(0))
EXPR(r_dbl_inc, DOUBLE(0)++)
EXPR(r_dbl_dec, DOUBLE(0)--)

ASSIGN(a_dbl_mul, *=)
ASSIGN(a_dbl_div, /=)
ASSIGN(a_dbl_add, +=)
ASSIGN(a_dbl_sub, -=)
#endif


/*	Functionstabelle
*/

static FuncDef_t fdef_double[] = {
	{ FUNC_VIRTUAL, &Type_double, "abs (double)", f_dbl_abs },
	{ FUNC_VIRTUAL, &Type_double, "min (double, double)", f_dbl_min },
	{ FUNC_VIRTUAL, &Type_double, "max (double, double)", f_dbl_max },
	{ FUNC_VIRTUAL, &Type_double, "adjust (double, str)", f_dbl_adjust },
	{ 0, &Type_double, "xdiv (double, double)", f_dbl_xdiv },
	{ 0, &Type_double, "xdiv1 (double, double)", f_dbl_xdiv1 },
	{ 0, &Type_double, "idx (double, double)", f_dbl_idx },
	{ 0, &Type_double, "rate (double, double)", f_dbl_rate },
	{ 0, &Type_int, "rnd (double)", f_dbl_rnd },

	{ FUNC_VIRTUAL, &Type_bool, "operator!() (double)", l_dbl_not },
	{ FUNC_VIRTUAL, &Type_double, "operator+() (double)", l_dbl_plus },
	{ FUNC_VIRTUAL, &Type_double, "operator-() (double)", l_dbl_minus },

	{ FUNC_VIRTUAL, &Type_double, "operator* (double, double)", b_dbl_mul },
	{ FUNC_VIRTUAL, &Type_double, "operator/ (double, double)", b_dbl_div },
	{ FUNC_VIRTUAL, &Type_double, "operator+ (double, double)", b_dbl_add },
	{ FUNC_VIRTUAL, &Type_double, "operator- (double, double)", b_dbl_sub },

	{ FUNC_VIRTUAL, &Type_bool, "operator< (double, double)", b_dbl_lt },
	{ FUNC_VIRTUAL, &Type_bool, "operator<= (double, double)", b_dbl_le },
	{ FUNC_VIRTUAL, &Type_bool, "operator== (double, double)", b_dbl_eq },
	{ FUNC_VIRTUAL, &Type_bool, "operator!= (double, double)", b_dbl_ne },
	{ FUNC_VIRTUAL, &Type_bool, "operator>= (double, double)", b_dbl_ge },
	{ FUNC_VIRTUAL, &Type_bool, "operator> (double, double)", b_dbl_gt },

#if	MAKE_ASSIGN_OP
	{ FUNC_VIRTUAL, &Type_double, "operator++ (double &)", r_dbl_inc },
	{ FUNC_VIRTUAL, &Type_double, "operator-- (double &)", r_dbl_dec },
	{ 0, &Type_double, "& double::operator++ & ()", l_dbl_inc },
	{ 0, &Type_double, "& double::operator-- & ()", l_dbl_dec },

	{ 0, &Type_double, "& double::operator*= & (double)", a_dbl_mul },
	{ 0, &Type_double, "& double::operator/= & (double)", a_dbl_div },
	{ 0, &Type_double, "& double::operator+= & (double)", a_dbl_add },
	{ 0, &Type_double, "& double::operator-= & (double)", a_dbl_sub },
#endif
};


/*	Initialisierung
*/

void CmdSetup_double(void)
{
	AddFuncDef(fdef_double, tabsize(fdef_double));
}
