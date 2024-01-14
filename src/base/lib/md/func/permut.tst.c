#include <EFEU/UnitTest.h>
#include <EFEU/mdmat.h>
#include "utest/eval.tst.h"

TEST_SUITE
{
	SetupMdMat();
	PRINT_EVAL("fmt_float = \"%#*.*f\"");
	PRINT_EVAL("float_prec = 2");

	PRINT_EVAL("mdmat a = mdcat(\"a\", mdcat(\"b\", 1))");
	PRINT_EVAL("mdmat x = mdcat(\"x\", mdcat(\"y\", a))");
	EXPECT_EVAL("x.print(cout);",
		"##MDMAT   2.0\n"
		"##Type    int\n"
		"##Locale  C\n"
		"##Rows    x y a b\n"
		"##Columns \n"
		"1 .\n"
		"x1.y1.a1.b1 1\n"
	);

	EXPECT_EVAL("x.permut(\"a,y\").print(cout);",
		"##MDMAT   2.0\n"
		"##Type    int\n"
		"##Locale  C\n"
		"##Rows    a y x b\n"
		"##Columns \n"
		"1 .\n"
		"a1.y1.x1.b1 1\n"
	);

	CLEANUP_EVAL;
}
