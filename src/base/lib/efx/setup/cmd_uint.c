/*
Vorzeichenfreie Ganzzahlobjekte

$Copyright (C) 2001 Erich Frühstück
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

/*	Funktionsmakros
*/

#define	UINT(n)	Val_uint(arg[n])
#define	URETVAL	Val_uint(rval)

#define	EXPR(name, expr)	\
CEXPR(name, URETVAL = expr)

#define	BINARY(name, op)	\
EXPR(name, UINT(0) op UINT(1))

#define	COMPARE(name, op)	\
CEXPR(name, Val_int(rval) = UINT(0) op UINT(1))

#define	ASSIGN(name, op)	\
CEXPR(name, UINT(0) op UINT(1))


/*	Funktionen
*/

static void f_uint_min (Func_t *func, void *rval, void **arg)
{
	register unsigned int a = UINT(0);
	register unsigned int b = UINT(1);
	URETVAL = a < b ? a : b;
}

static void f_uint_max (Func_t *func, void *rval, void **arg)
{
	register unsigned int a = UINT(0);
	register unsigned int b = UINT(1);
	URETVAL = a > b ? a : b;
}


/*	Unäre Operatoren
*/

EXPR(l_uint_not, !UINT(0))
EXPR(l_uint_cpl, ~UINT(0))
EXPR(l_uint_minus, -UINT(0))
EXPR(l_uint_plus, UINT(0))

/*	Binäre Operatoren
*/

BINARY(b_uint_mul, *)
BINARY(b_uint_div, /)
BINARY(b_uint_mod, %)
BINARY(b_uint_add, +)
BINARY(b_uint_sub, -)
BINARY(b_uint_lshift, <<)
BINARY(b_uint_rshift, >>)
BINARY(b_uint_and, &)
BINARY(b_uint_xor, ^)
BINARY(b_uint_or, |)

/*	Vergleichsoperatoren
*/

COMPARE(b_uint_lt, <)
COMPARE(b_uint_le, <=)
COMPARE(b_uint_eq, ==)
COMPARE(b_uint_ne, !=)
COMPARE(b_uint_ge, >=)
COMPARE(b_uint_gt, >)

/*	Zuweisungsoperatoren
*/

#if	MAKE_ASSIGN_OP

EXPR(r_uint_inc, UINT(0)++)
EXPR(r_uint_dec, UINT(0)--)
CEXPR(l_uint_inc, ++UINT(0))
CEXPR(l_uint_dec, --UINT(0))

ASSIGN(a_uint_mul, *=)
ASSIGN(a_uint_div, /=)
ASSIGN(a_uint_mod, %=)
ASSIGN(a_uint_add, +=)
ASSIGN(a_uint_sub, -=)
ASSIGN(a_uint_lshift, <<=)
ASSIGN(a_uint_rshift, >>=)
ASSIGN(a_uint_and, &=)
ASSIGN(a_uint_xor, ^=)
ASSIGN(a_uint_or, |=)

#endif


/*	Funktionstabelle
*/

static FuncDef_t fdef_uint[] = {
	{ FUNC_VIRTUAL, &Type_uint, "min (unsigned, unsigned)", f_uint_min },
	{ FUNC_VIRTUAL, &Type_uint, "max (unsigned, unsigned)", f_uint_max },

	{ FUNC_VIRTUAL, &Type_bool, "operator!() (unsigned)", l_uint_not },
	{ FUNC_VIRTUAL, &Type_uint, "operator~() (unsigned)", l_uint_cpl },
	{ FUNC_VIRTUAL, &Type_uint, "operator+() (unsigned)", l_uint_plus },
	{ FUNC_VIRTUAL, &Type_uint, "operator-() (unsigned)", l_uint_minus },

	{ FUNC_VIRTUAL, &Type_uint, "operator* (unsigned, unsigned)", b_uint_mul },
	{ FUNC_VIRTUAL, &Type_uint, "operator/ (unsigned, unsigned)", b_uint_div },
	{ FUNC_VIRTUAL, &Type_uint, "operator% (unsigned, unsigned)", b_uint_mod },
	{ FUNC_VIRTUAL, &Type_uint, "operator+ (unsigned, unsigned)", b_uint_add },
	{ FUNC_VIRTUAL, &Type_uint, "operator- (unsigned, unsigned)", b_uint_sub },
	{ FUNC_VIRTUAL, &Type_uint, "operator<< (unsigned, unsigned)", b_uint_lshift },
	{ FUNC_VIRTUAL, &Type_uint, "operator>> (unsigned, unsigned)", b_uint_rshift },
	{ FUNC_VIRTUAL, &Type_uint, "operator& (unsigned, unsigned)", b_uint_and },
	{ FUNC_VIRTUAL, &Type_uint, "operator^ (unsigned, unsigned)", b_uint_xor },
	{ FUNC_VIRTUAL, &Type_uint, "operator| (unsigned, unsigned)", b_uint_or },

	{ FUNC_VIRTUAL, &Type_bool, "operator< (unsigned, unsigned)", b_uint_lt },
	{ FUNC_VIRTUAL, &Type_bool, "operator<= (unsigned, unsigned)", b_uint_le },
	{ FUNC_VIRTUAL, &Type_bool, "operator== (unsigned, unsigned)", b_uint_eq },
	{ FUNC_VIRTUAL, &Type_bool, "operator!= (unsigned, unsigned)", b_uint_ne },
	{ FUNC_VIRTUAL, &Type_bool, "operator>= (unsigned, unsigned)", b_uint_ge },
	{ FUNC_VIRTUAL, &Type_bool, "operator> (unsigned, unsigned)", b_uint_gt },

#if	MAKE_ASSIGN_OP
	{ FUNC_VIRTUAL, &Type_uint, "operator++ (unsigned &)", r_uint_inc },
	{ FUNC_VIRTUAL, &Type_uint, "operator-- (unsigned &)", r_uint_dec },
	{ 0, &Type_uint, "& unsigned::operator++ & ()", l_uint_inc },
	{ 0, &Type_uint, "& unsigned::operator-- & ()", l_uint_dec },

	{ 0, &Type_uint, "& unsigned::operator*= & (unsigned)", a_uint_mul },
	{ 0, &Type_uint, "& unsigned::operator/= & (unsigned)", a_uint_div },
	{ 0, &Type_uint, "& unsigned::operator%= & (unsigned)", a_uint_mod },
	{ 0, &Type_uint, "& unsigned::operator+= & (unsigned)", a_uint_add },
	{ 0, &Type_uint, "& unsigned::operator-= & (unsigned)", a_uint_sub },
	{ 0, &Type_uint, "& unsigned::operator<<= & (unsigned)", a_uint_lshift },
	{ 0, &Type_uint, "& unsigned::operator>>= & (unsigned)", a_uint_rshift },
	{ 0, &Type_uint, "& unsigned::operator&= & (unsigned)", a_uint_and },
	{ 0, &Type_uint, "& unsigned::operator^= & (unsigned)", a_uint_xor },
	{ 0, &Type_uint, "& unsigned::operator|= & (unsigned)", a_uint_or },
#endif
};


/*	Initialisierung
*/

void CmdSetup_uint(void)
{
	AddFuncDef(fdef_uint, tabsize(fdef_uint));
}
