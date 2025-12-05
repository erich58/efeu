#include <EFEU/UnitTest.h>
#include <EFEU/mdmat.h>
#include <EFEU/utest/eval.tst.h>

TEST_SUITE
{
	SetupMdMat();

	PRINT_EVAL("fmt_float = \"%#*.*f\"");
	PRINT_EVAL("float_prec = 2");

	md_round(NULL, 0., 0);
	md_adjust(NULL, 0., 0);
	EXPECT_EVAL("mdcat(NULL, 1.7, 3.4).round(1.).print(cout, \"nohead\");",
		"1 .\n"
		"X1 2.00\n"
		"X2 3.00\n"
	);

	EXPECT_EVAL("mdcat(NULL, 1.7, 3.3).round(-2.).print(cout, \"nohead\");",
		"1 .\n"
		"X1 -3.00\n"
		"X2 -7.00\n"
	);

	EXPECT_EVAL("mdcat(NULL, 1.7, 3.4).adjust(8.).print(cout, \"nohead\");",
		"1 .\n"
		"X1 3.00\n"
		"X2 5.00\n"
	);
	CLEANUP_EVAL;
}
