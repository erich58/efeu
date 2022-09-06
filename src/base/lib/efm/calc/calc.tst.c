#include <EFEU/UnitTest.h>
#include <EFEU/calculator.h>
#include "utest/in.tst.h"

#define	CALC_REM(input, val, rem)	\
IO2STR_FUNCTION(calc_eval, (NULL, in, NULL), input, val, rem)

#define	CALC(input, val)	CALC_REM(input, val, NULL)

TEST_DECL(empty)
{
	CALC(NULL, NULL);
	CALC("", NULL);
	CALC_REM("\n", NULL, "\n");
}

TEST_DECL(obj)
{
	CALC("x", "0");
	CALC("42", "42");
	CALC("042", "34");
	CALC("0x42", "66");
	CALC("\"string\"", "string");
	CALC("\"abc\" \"xyz\" ", "abcxyz");
	CALC(" 3 \"abc\" ", "3abc");
	CALC(" - 3 \"abc\" ", "-3abc");
}

TEST_DECL(leftop_no_obj)
{
	CALC_REM(" + ", "0", "");
	CALC_REM(" - ", "0", "");
	CALC_REM(" ! ", "1", "");

	CALC_REM(" ( ) ", NULL, "");
	CALC_REM(" ( + ) ", "0", "");
	CALC_REM(" ( - ) ", "0", "");
	CALC_REM(" ( ! ) ", "1", "");
}

TEST_DECL(leftop)
{
	CALC("x", "0");
	CALC("-42", "-42");
	CALC("-+- + 42", "42");
	CALC("+\"string\"", "0");
	CALC("-\" -12\"", "12");
	CALC("!\"string\"", "1");
	CALC("!42", "0");
	CALC("!!42", "1");
}

TEST_DECL(equal)
{
	CALC("42 == 42", "1");
	CALC("42 == 0", "0");
	CALC("42 != 42", "0");
	CALC("42 != 0", "1");
	CALC("42 == \"42\"", "1");
	CALC("\"abc\" == \"abc\"", "1");
	CALC("\"abc\" != \"abc\"", "0");
	CALC("0x1 == 1", "1");
	CALC_REM("42 = 15", "42", "= 15");
	CALC_REM("42 ! 15", "42", "! 15");
}

TEST_DECL(cmp)
{
	CALC("1 < 1", "0");
	CALC("1 <= 1", "1");
	CALC("1 >= 1", "1");
	CALC("1 > 1", "0");
}

TEST_DECL(numop)
{
	CALC("1 + 1", "2");
	CALC("5 - 7", "-2");
	CALC("3 * 5", "15");
	CALC("7 / 2", "3");
	CALC("1 / 0", "0");
	CALC("0 / 0", "0");
	CALC("5 % 2", "1");
	CALC("5 % 0", "0");
	CALC("0 % 0", "0");
	CALC("3 + 4 * 5", "23");
	CALC("(3 + 4) * 5", "35");
}

TEST_SUITE
{
	TEST_RUN(empty);
	TEST_RUN(obj);
	TEST_RUN(leftop_no_obj);
	TEST_RUN(leftop);
	TEST_RUN(equal);
	TEST_RUN(cmp);
	TEST_RUN(numop);
	CLEANUP_IN;
}
