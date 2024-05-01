#include <EFEU/UnitTest.h>
#include <EFEU/mdmat.h>
#include "utest/eval.tst.h"

TEST_SUITE
{
	SetupMdMat();

	PRINT_EVAL("fmt_float = \"%#*.*f\"");
	PRINT_EVAL("float_prec = 2");

	EXPECT_EVAL("sum(mdcat(NULL, 1.7, 3.4))", "5.10");

	CLEANUP_EVAL;
}
