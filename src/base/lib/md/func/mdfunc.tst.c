#include <EFEU/UnitTest.h>
#include <EFEU/mdmat.h>
#include "utest/eval.tst.h"

TEST_SUITE
{
	SetupMdMat();
	TEST_FILE("a",
		"##MDMAT   2.0\n"
		"##Title    Data\n"
		"##Type    int x\n"
		"##Locale  C\n"
		"##Rows    r\n"
		"##Columns c\n"
		"2 x y\n"
		"a 1 2\n"
		"b 3 0\n"
	);
	EXPECT_EVAL(UnitTest_tpl("str tmp = \"@\""),
		UnitTest_qstr(UnitTest_tmp(NULL)));
	EXPECT_EVAL("mdmat a = mdread(findopen(tmp, \"a\"))",
		"MD: name=\"Data\", type=int, dim=2, size=16");
	PRINT_EVAL("a.save(tmp + \"/b\")");
	PRINT_EVAL("a.map(tmp + \"/c\")");

	EXPECT_EVAL("mdfile(cout, tmp + \"/b\");",
		UnitTest_tpl("@/b: int[4=2*2] \"Data\"\n"));
	EXPECT_EVAL("mdfile(cout, tmp + \"/c\");",
		UnitTest_tpl("@/c: int[4=2*2] \"Data\"\n"));

	EXPECT_EVAL("mdmat b = mdload(tmp + \"/b\")",
		"MD: name=\"Data\", type=int, dim=2, size=16");
	EXPECT_EVAL("b.mark(\"c\").print(cout, \"nohead\");",
		"2 x y\n"
		"a 1 2\n"
		"b 3 0\n"
	);
	EXPECT_EVAL("mdmat c = reload(b, \"c=x\")",
		"MD: name=\"Data\", type=int, dim=2, size=8");
	EXPECT_EVAL("c.mark(\"c\").print(cout, \"nohead\");",
		"1 x\n"
		"a 1\n"
		"b 3\n"
	);

	EXPECT_EVAL("mdcpy(a).mark(\"c\").print(cout, \"nohead\");",
		"2 x y\n"
		"a 1 2\n"
		"b 3 0\n"
	);
	EXPECT_EVAL("mdcpy(a, \"c=x\").mark(\"c\").print(cout, \"nohead\");",
		"1 x\n"
		"a 1\n"
		"b 3\n"
	);

	PRINT_EVAL("mdmat x = NULL");
	EXPECT_EVAL("x.paste(\"c\", 0, 0)", "NULL");
	PRINT_EVAL("x = mdcpy(a)");
	EXPECT_EVAL("x.paste(\"z\", 0, 0, \"-\")",
		"MD: name=NULL, type=int, dim=1, size=16");
	EXPECT_EVAL("x.print(cout, \"nohead\");",
		"1 .\n"
		"a-x 1\n"
		"a-y 2\n"
		"b-x 3\n"
		"b-y 0\n"
	);

	PRINT_EVAL("x = mdcpy(a)");
	PRINT_EVAL("x.mark(\"c\")");
	EXPECT_EVAL("mdmul(x, x).print(cout, \"nohead\");",
		"2 a b\n"
		"a 5 3\n"
		"b 3 9\n"
	);

	PRINT_EVAL("x = mdcpy(a)");
	PRINT_EVAL("x.mark(\"r\")");
	EXPECT_EVAL("mdprod(x).print(cout, \"nohead\");",
		"2 a b\n"
		"a 5 3\n"
		"b 3 9\n"
	);

	EXPECT_EVAL("a = NULL", "NULL");
	EXPECT_EVAL("a += a", "NULL");
	EXPECT_EVAL("a + a", "NULL");
	EXPECT_EVAL("a.expr(\"+\", a)", "NULL");
	EXPECT_EVAL("a.expr(\"++\")", "NULL");
	EXPECT_EVAL("mdexpr(operator\"+\", a, a)", "NULL");
	EXPECT_EVAL("mdexpr(operator\"++\", a)", "NULL");
	CLEANUP_EVAL;
}
