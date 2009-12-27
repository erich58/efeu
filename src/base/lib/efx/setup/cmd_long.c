/*	Ganzzahlobjekte
	(c) 1994 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6

	Version 0.4
*/

#include <EFEU/object.h>
#include <EFEU/cmdconfig.h>

/*	Funktionsmakros
*/

#define	LONG(n)	Val_long(arg[n])
#define	LRETVAL	Val_long(rval)

#define	EXPR(name, expr)	\
CEXPR(name, LRETVAL = expr)

#define	BINARY(name, op)	\
EXPR(name, LONG(0) op LONG(1))

#define	COMPARE(name, op)	\
CEXPR(name, Val_int(rval) = LONG(0) op LONG(1))

#define	ASSIGN(name, op)	\
LEXPR(name, LONG(0) op LONG(1))


/*	Funktionen
*/

static void f_long_abs (Func_t *func, void *rval, void **arg)
{
	register int a = LONG(0);
	LRETVAL = a < 0 ? -a : a;
}

static void f_long_min (Func_t *func, void *rval, void **arg)
{
	register int a = LONG(0);
	register int b = LONG(1);
	LRETVAL = a < b ? a : b;
}

static void f_long_max (Func_t *func, void *rval, void **arg)
{
	register int a = LONG(0);
	register int b = LONG(1);
	LRETVAL = a > b ? a : b;
}


/*	Unäre Operatoren
*/

EXPR(l_long_not, !LONG(0))
EXPR(l_long_cpl, ~LONG(0))
EXPR(l_long_minus, -LONG(0))
EXPR(l_long_plus, LONG(0))

/*	Binäre Operatoren
*/

BINARY(b_long_mul, *)
BINARY(b_long_div, /)
BINARY(b_long_mod, %)
BINARY(b_long_add, +)
BINARY(b_long_sub, -)
BINARY(b_long_lshift, <<)
BINARY(b_long_rshift, >>)
BINARY(b_long_and, &)
BINARY(b_long_xor, ^)
BINARY(b_long_or, |)

/*	Vergleichsoperatoren
*/

COMPARE(b_long_lt, <)
COMPARE(b_long_le, <=)
COMPARE(b_long_eq, ==)
COMPARE(b_long_ne, !=)
COMPARE(b_long_ge, >=)
COMPARE(b_long_gt, >)

/*	Zuweisungsoperatoren
*/

#if	MAKE_ASSIGN_OP

EXPR(r_long_inc, LONG(0)++)
EXPR(r_long_dec, LONG(0)--)
LEXPR(l_long_inc, ++LONG(0))
LEXPR(l_long_dec, --LONG(0))

ASSIGN(a_long_mul, *=)
ASSIGN(a_long_div, /=)
ASSIGN(a_long_mod, %=)
ASSIGN(a_long_add, +=)
ASSIGN(a_long_sub, -=)
ASSIGN(a_long_lshift, <<=)
ASSIGN(a_long_rshift, >>=)
ASSIGN(a_long_and, &=)
ASSIGN(a_long_xor, ^=)
ASSIGN(a_long_or, |=)

#endif


/*	Funktionstabelle
*/

static FuncDef_t fdef_long[] = {
	{ FUNC_VIRTUAL, &Type_long, "abs (long)", f_long_abs },
	{ FUNC_VIRTUAL, &Type_long, "min (long, long)", f_long_min },
	{ FUNC_VIRTUAL, &Type_long, "max (long, long)", f_long_max },

	{ FUNC_VIRTUAL, &Type_bool, "operator!() (long)", l_long_not },
	{ FUNC_VIRTUAL, &Type_long, "operator~() (long)", l_long_cpl },
	{ FUNC_VIRTUAL, &Type_long, "operator+() (long)", l_long_plus },
	{ FUNC_VIRTUAL, &Type_long, "operator-() (long)", l_long_minus },

	{ FUNC_VIRTUAL, &Type_long, "operator* (long, long)", b_long_mul },
	{ FUNC_VIRTUAL, &Type_long, "operator/ (long, long)", b_long_div },
	{ FUNC_VIRTUAL, &Type_long, "operator% (long, long)", b_long_mod },
	{ FUNC_VIRTUAL, &Type_long, "operator+ (long, long)", b_long_add },
	{ FUNC_VIRTUAL, &Type_long, "operator- (long, long)", b_long_sub },
	{ FUNC_VIRTUAL, &Type_long, "operator<< (long, long)", b_long_lshift },
	{ FUNC_VIRTUAL, &Type_long, "operator>> (long, long)", b_long_rshift },
	{ FUNC_VIRTUAL, &Type_long, "operator& (long, long)", b_long_and },
	{ FUNC_VIRTUAL, &Type_long, "operator^ (long, long)", b_long_xor },
	{ FUNC_VIRTUAL, &Type_long, "operator| (long, long)", b_long_or },

	{ FUNC_VIRTUAL, &Type_bool, "operator< (long, long)", b_long_lt },
	{ FUNC_VIRTUAL, &Type_bool, "operator<= (long, long)", b_long_le },
	{ FUNC_VIRTUAL, &Type_bool, "operator== (long, long)", b_long_eq },
	{ FUNC_VIRTUAL, &Type_bool, "operator!= (long, long)", b_long_ne },
	{ FUNC_VIRTUAL, &Type_bool, "operator>= (long, long)", b_long_ge },
	{ FUNC_VIRTUAL, &Type_bool, "operator> (long, long)", b_long_gt },

#if	MAKE_ASSIGN_OP
	{ FUNC_VIRTUAL, &Type_long, "operator++ (long &)", r_long_inc },
	{ FUNC_VIRTUAL, &Type_long, "operator-- (long &)", r_long_dec },
	{ 0, &Type_long, "& long::operator++ & ()", l_long_inc },
	{ 0, &Type_long, "& long::operator-- & ()", l_long_dec },

	{ 0, &Type_long, "& long::operator*= & (long)", a_long_mul },
	{ 0, &Type_long, "& long::operator/= & (long)", a_long_div },
	{ 0, &Type_long, "& long::operator%= & (long)", a_long_mod },
	{ 0, &Type_long, "& long::operator+= & (long)", a_long_add },
	{ 0, &Type_long, "& long::operator-= & (long)", a_long_sub },
	{ 0, &Type_long, "& long::operator<<= & (long)", a_long_lshift },
	{ 0, &Type_long, "& long::operator>>= & (long)", a_long_rshift },
	{ 0, &Type_long, "& long::operator&= & (long)", a_long_and },
	{ 0, &Type_long, "& long::operator^= & (long)", a_long_xor },
	{ 0, &Type_long, "& long::operator|= & (long)", a_long_or },
#endif
};


/*	Initialisierung
*/

void CmdSetup_long(void)
{
	AddFuncDef(fdef_long, tabsize(fdef_long));
}
