#include <EFEU/UnitTest.h>
#include <EFEU/mdmat.h>
#include "utest/eval.tst.h"

TEST_SUITE
{
	SetupMdMat();
	PRINT_EVAL("fmt_float = \"%#*.*f\"");
	PRINT_EVAL("float_prec = 2");

	PRINT_EVAL("mdmat a = mdcat(\"a\", 1, 2))");
	PRINT_EVAL("mdmat b = mdcat(\"a\", 3, 4))");
	PRINT_EVAL("mdmat x = mdcat(\"x\", a, b)");
	EXPECT_EVAL("x.mark(\"#-1\").print(cout);",
		"##MDMAT   2.0\n"
		"##Type    int\n"
		"##Locale  C\n"
		"##Rows    x\n"
		"##Columns a\n"
		"2 a1 a2\n"
		"x1 1 2\n"
		"x2 3 4\n"
	);

	EXPECT_EVAL("x.select().eval(\"-\", "
		"[fprintf(cout, \"%s.%s %d\n\", x, a, this);]);",
		"x1.a1 1\n"
		"x1.a2 2\n"
		"x2.a1 3\n"
		"x2.a2 4\n"
	);

	EXPECT_EVAL("x.select(\"a=a1\").eval(\"-\", "
		"[fprintf(cout, \"%s.%s %d\n\", x, a, this);]);",
		"x1.a1 1\n"
		"x2.a1 3\n"
	);

	EXPECT_EVAL("x.select().eval(\"a\", "
		"[fprintf(cout, \"%s %d %d\n\", x, a1, a2);]);",
		"x1 1 2\n"
		"x2 3 4\n"
	);

	EXPECT_EVAL("x.select(\"a=a1\").eval(\"a\", "
		"[fprintf(cout, \"%s %d\n\", x, a1);]);",
		"x1 1\n"
		"x2 3\n"
	);

	CLEANUP_EVAL;
}
