/*
Ganzzahlobjekte

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

#define	INT(n)	Val_int(arg[n])
#define	STR(n)	Val_str(arg[n])
#define	IRETVAL	Val_int(rval)

#define	EXPR(name, expr)	\
CEXPR(name, IRETVAL = expr)

#define	BINARY(name, op)	\
EXPR(name, INT(0) op INT(1))

#define	COMPARE(name, op)	\
CEXPR(name, IRETVAL = INT(0) op INT(1))

#define	ASSIGN(name, op)	\
CEXPR(name, INT(0) op INT(1))


/*	Funktionen
*/

static void f_int_abs (Func_t *func, void *rval, void **arg)
{
	register int a = INT(0);
	IRETVAL = a < 0 ? -a : a;
}

static void f_int_min (Func_t *func, void *rval, void **arg)
{
	register int a = INT(0);
	register int b = INT(1);
	IRETVAL = a < b ? a : b;
}

static void f_int_max (Func_t *func, void *rval, void **arg)
{
	register int a = INT(0);
	register int b = INT(1);
	IRETVAL = a > b ? a : b;
}

static void f_int_adjust (Func_t *func, void *rval, void **arg)
{
	register int a = INT(0);
	register char *b = STR(1);

	if	(b == NULL)
	{
		IRETVAL = a;
		return;
	}

	while (isspace(*b))
		b++;

	if	(*b == '+')	IRETVAL = a + strtol(b + 1, NULL, 0);
	else if	(*b == '-')	IRETVAL = a - strtol(b + 1, NULL, 0);
	else if	(isdigit(*b))	IRETVAL = strtol(b, NULL, 0);
	else			IRETVAL = a;
}

/*	Testfunktionen
*/

EXPR(f_isdigit, isdigit(INT(0)) ? 1 : 0)
EXPR(f_isalpha, isalpha(INT(0)) ? 1 : 0)
EXPR(f_isalnum, isalnum(INT(0)) ? 1 : 0)
EXPR(f_isspace, isspace(INT(0)) ? 1 : 0)

/*	Unäre Operatoren
*/

EXPR(l_int_not, !INT(0))
EXPR(l_int_cpl, ~INT(0))
EXPR(l_int_minus, -INT(0))
EXPR(l_int_plus, INT(0))

/*	Binäre Operatoren
*/

BINARY(b_int_mul, *)
BINARY(b_int_div, /)
BINARY(b_int_mod, %)
BINARY(b_int_add, +)
BINARY(b_int_sub, -)
BINARY(b_int_lshift, <<)
BINARY(b_int_rshift, >>)
BINARY(b_int_and, &)
BINARY(b_int_xor, ^)
BINARY(b_int_or, |)

/*	Vergleichsoperatoren
*/

COMPARE(b_int_lt, <)
COMPARE(b_int_le, <=)
COMPARE(b_int_eq, ==)
COMPARE(b_int_ne, !=)
COMPARE(b_int_ge, >=)
COMPARE(b_int_gt, >)

/*	Zuweisungsoperatoren
*/

#if	MAKE_ASSIGN_OP

CEXPR(l_int_inc, ++INT(0))
CEXPR(l_int_dec, --INT(0))
EXPR(r_int_inc, INT(0)++)
EXPR(r_int_dec, INT(0)--)

ASSIGN(a_int_mul, *=)
ASSIGN(a_int_div, /=)
ASSIGN(a_int_mod, %=)
ASSIGN(a_int_add, +=)
ASSIGN(a_int_sub, -=)
ASSIGN(a_int_lshift, <<=)
ASSIGN(a_int_rshift, >>=)
ASSIGN(a_int_and, &=)
ASSIGN(a_int_xor, ^=)
ASSIGN(a_int_or, |=)

#endif


/*	Funktionstabelle
*/

static FuncDef_t fdef_int[] = {
	{ FUNC_VIRTUAL, &Type_int, "abs (int)", f_int_abs },
	{ FUNC_VIRTUAL, &Type_int, "min (int, int)", f_int_min },
	{ FUNC_VIRTUAL, &Type_int, "max (int, int)", f_int_max },
	{ FUNC_VIRTUAL, &Type_int, "adjust (int, str)", f_int_adjust },

	{ FUNC_VIRTUAL, &Type_bool, "isdigit (int)", f_isdigit },
	{ FUNC_VIRTUAL, &Type_bool, "isalpha (int)", f_isalpha },
	{ FUNC_VIRTUAL, &Type_bool, "isalnum (int)", f_isalnum },
	{ FUNC_VIRTUAL, &Type_bool, "isspace (int)", f_isspace },

	{ FUNC_VIRTUAL, &Type_bool, "operator!() (int)", l_int_not },
	{ FUNC_VIRTUAL, &Type_int, "operator~() (int)", l_int_cpl },
	{ FUNC_VIRTUAL, &Type_int, "operator+() (int)", l_int_plus },
	{ FUNC_VIRTUAL, &Type_int, "operator-() (int)", l_int_minus },

	{ FUNC_VIRTUAL, &Type_int, "operator* (int, int)", b_int_mul },
	{ FUNC_VIRTUAL, &Type_int, "operator/ (int, int)", b_int_div },
	{ FUNC_VIRTUAL, &Type_int, "operator% (int, int)", b_int_mod },
	{ FUNC_VIRTUAL, &Type_int, "operator+ (int, int)", b_int_add },
	{ FUNC_VIRTUAL, &Type_int, "operator- (int, int)", b_int_sub },
	{ FUNC_VIRTUAL, &Type_int, "operator<< (int, int)", b_int_lshift },
	{ FUNC_VIRTUAL, &Type_int, "operator>> (int, int)", b_int_rshift },
	{ FUNC_VIRTUAL, &Type_int, "operator& (int, int)", b_int_and },
	{ FUNC_VIRTUAL, &Type_int, "operator^ (int, int)", b_int_xor },
	{ FUNC_VIRTUAL, &Type_int, "operator| (int, int)", b_int_or },

	{ FUNC_VIRTUAL, &Type_bool, "operator< (int, int)", b_int_lt },
	{ FUNC_VIRTUAL, &Type_bool, "operator<= (int, int)", b_int_le },
	{ FUNC_VIRTUAL, &Type_bool, "operator== (int, int)", b_int_eq },
	{ FUNC_VIRTUAL, &Type_bool, "operator!= (int, int)", b_int_ne },
	{ FUNC_VIRTUAL, &Type_bool, "operator>= (int, int)", b_int_ge },
	{ FUNC_VIRTUAL, &Type_bool, "operator> (int, int)", b_int_gt },

#if	MAKE_ASSIGN_OP
	{ FUNC_VIRTUAL, &Type_int, "operator++ (int &)", r_int_inc },
	{ FUNC_VIRTUAL, &Type_int, "operator-- (int &)", r_int_dec },
	{ 0, &Type_int, "& int::operator++ & ()", l_int_inc },
	{ 0, &Type_int, "& int::operator-- & ()", l_int_dec },
	
	{ 0, &Type_int, "& int::operator*= & (int)", a_int_mul },
	{ 0, &Type_int, "& int::operator/= & (int)", a_int_div },
	{ 0, &Type_int, "& int::operator%= & (int)", a_int_mod },
	{ 0, &Type_int, "& int::operator+= & (int)", a_int_add },
	{ 0, &Type_int, "& int::operator-= & (int)", a_int_sub },
	{ 0, &Type_int, "& int::operator<<= & (int)", a_int_lshift },
	{ 0, &Type_int, "& int::operator>>= & (int)", a_int_rshift },
	{ 0, &Type_int, "& int::operator&= & (int)", a_int_and },
	{ 0, &Type_int, "& int::operator^= & (int)", a_int_xor },
	{ 0, &Type_int, "& int::operator|= & (int)", a_int_or },
#endif
};


/*	Initialisierung
*/

void CmdSetup_int(void)
{
	AddFuncDef(fdef_int, tabsize(fdef_int));
}
