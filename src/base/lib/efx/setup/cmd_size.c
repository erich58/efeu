/*
Vorzeichenfreie lange Ganzzahlobjekte

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

#define	SIZE(n)	Val_size(arg[n])
#define	SRETVAL	Val_size(rval)

#define	EXPR(name, expr)	\
CEXPR(name, SRETVAL = expr)

#define	BINARY(name, op)	\
EXPR(name, SIZE(0) op SIZE(1))

#define	COMPARE(name, op)	\
CEXPR(name, Val_int(rval) = SIZE(0) op SIZE(1))

#define	ASSIGN(name, op)	\
CEXPR(name, SIZE(0) op SIZE(1))


/*	Funktionen
*/

static void f_size_min (EfiFunc *func, void *rval, void **arg)
{
	register unsigned long a = SIZE(0);
	register unsigned long b = SIZE(1);
	SRETVAL = a < b ? a : b;
}

static void f_size_max (EfiFunc *func, void *rval, void **arg)
{
	register unsigned long a = SIZE(0);
	register unsigned long b = SIZE(1);
	SRETVAL = a > b ? a : b;
}


/*	Unäre Operatoren
*/

EXPR(l_size_not, !SIZE(0))
EXPR(l_size_cpl, ~SIZE(0))
EXPR(l_size_minus, -SIZE(0))
EXPR(l_size_plus, SIZE(0))

/*	Binäre Operatoren
*/

BINARY(b_size_mul, *)
BINARY(b_size_div, /)
BINARY(b_size_mod, %)
BINARY(b_size_add, +)
BINARY(b_size_sub, -)
BINARY(b_size_lshift, <<)
BINARY(b_size_rshift, >>)
BINARY(b_size_and, &)
BINARY(b_size_xor, ^)
BINARY(b_size_or, |)

/*	Vergleichsoperatoren
*/

COMPARE(b_size_lt, <)
COMPARE(b_size_le, <=)
COMPARE(b_size_eq, ==)
COMPARE(b_size_ne, !=)
COMPARE(b_size_ge, >=)
COMPARE(b_size_gt, >)

/*	Zuweisungsoperatoren
*/

#if	MAKE_ASSIGN_OP

EXPR(r_size_inc, SIZE(0)++)
EXPR(r_size_dec, SIZE(0)--)
CEXPR(l_size_inc, ++SIZE(0))
CEXPR(l_size_dec, --SIZE(0))

ASSIGN(a_size_mul, *=)
ASSIGN(a_size_div, /=)
ASSIGN(a_size_mod, %=)
ASSIGN(a_size_add, +=)
ASSIGN(a_size_sub, -=)
ASSIGN(a_size_lshift, <<=)
ASSIGN(a_size_rshift, >>=)
ASSIGN(a_size_and, &=)
ASSIGN(a_size_xor, ^=)
ASSIGN(a_size_or, |=)

#endif


/*	Funktionstabelle
*/

static EfiFuncDef fdef_size[] = {
	{ FUNC_VIRTUAL, &Type_size, "min (size_t, size_t)", f_size_min },
	{ FUNC_VIRTUAL, &Type_size, "max (size_t, size_t)", f_size_max },

	{ FUNC_VIRTUAL, &Type_bool, "operator!() (size_t)", l_size_not },
	{ FUNC_VIRTUAL, &Type_size, "operator~() (size_t)", l_size_cpl },
	{ FUNC_VIRTUAL, &Type_size, "operator+() (size_t)", l_size_plus },
	{ FUNC_VIRTUAL, &Type_size, "operator-() (size_t)", l_size_minus },

	{ FUNC_VIRTUAL, &Type_size, "operator* (size_t, size_t)", b_size_mul },
	{ FUNC_VIRTUAL, &Type_size, "operator/ (size_t, size_t)", b_size_div },
	{ FUNC_VIRTUAL, &Type_size, "operator% (size_t, size_t)", b_size_mod },
	{ FUNC_VIRTUAL, &Type_size, "operator+ (size_t, size_t)", b_size_add },
	{ FUNC_VIRTUAL, &Type_size, "operator- (size_t, size_t)", b_size_sub },
	{ FUNC_VIRTUAL, &Type_size, "operator<< (size_t, size_t)", b_size_lshift },
	{ FUNC_VIRTUAL, &Type_size, "operator>> (size_t, size_t)", b_size_rshift },
	{ FUNC_VIRTUAL, &Type_size, "operator& (size_t, size_t)", b_size_and },
	{ FUNC_VIRTUAL, &Type_size, "operator^ (size_t, size_t)", b_size_xor },
	{ FUNC_VIRTUAL, &Type_size, "operator| (size_t, size_t)", b_size_or },

	{ FUNC_VIRTUAL, &Type_bool, "operator< (size_t, size_t)", b_size_lt },
	{ FUNC_VIRTUAL, &Type_bool, "operator<= (size_t, size_t)", b_size_le },
	{ FUNC_VIRTUAL, &Type_bool, "operator== (size_t, size_t)", b_size_eq },
	{ FUNC_VIRTUAL, &Type_bool, "operator!= (size_t, size_t)", b_size_ne },
	{ FUNC_VIRTUAL, &Type_bool, "operator>= (size_t, size_t)", b_size_ge },
	{ FUNC_VIRTUAL, &Type_bool, "operator> (size_t, size_t)", b_size_gt },

#if	MAKE_ASSIGN_OP
	{ FUNC_VIRTUAL, &Type_size, "operator++ (size_t &)", r_size_inc },
	{ FUNC_VIRTUAL, &Type_size, "operator-- (size_t &)", r_size_dec },
	{ 0, &Type_size, "& size_t::operator++ & ()", l_size_inc },
	{ 0, &Type_size, "& size_t::operator-- & ()", l_size_dec },

	{ 0, &Type_size, "& size_t::operator*= & (size_t)", a_size_mul },
	{ 0, &Type_size, "& size_t::operator/= & (size_t)", a_size_div },
	{ 0, &Type_size, "& size_t::operator%= & (size_t)", a_size_mod },
	{ 0, &Type_size, "& size_t::operator+= & (size_t)", a_size_add },
	{ 0, &Type_size, "& size_t::operator-= & (size_t)", a_size_sub },
	{ 0, &Type_size, "& size_t::operator<<= & (size_t)", a_size_lshift },
	{ 0, &Type_size, "& size_t::operator>>= & (size_t)", a_size_rshift },
	{ 0, &Type_size, "& size_t::operator&= & (size_t)", a_size_and },
	{ 0, &Type_size, "& size_t::operator^= & (size_t)", a_size_xor },
	{ 0, &Type_size, "& size_t::operator|= & (size_t)", a_size_or },
#endif
};


/*	Initialisierung
*/

void CmdSetup_size(void)
{
	AddFuncDef(fdef_size, tabsize(fdef_size));
}
