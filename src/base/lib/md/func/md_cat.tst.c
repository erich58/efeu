#include <EFEU/UnitTest.h>
#include <EFEU/mdmat.h>
#include <EFEU/utest/eval.tst.h>

TEST_SUITE
{
	SetupMdMat();
	PRINT_EVAL("fmt_float = \"%#*.*f\"");
	PRINT_EVAL("float_prec = 2");

	EXPECT(md_cat(NULL, NULL, 0) == NULL);

	EXPECT_EVAL("mdmat a = 1; a.title = \"A\"", "\"A\"");
	EXPECT_EVAL("mdmat b = 2; b.title = \"B\"", "\"B\"");

	EXPECT_EVAL("mdcat(NULL, a, b, 5.).print(cout);",
		"##MDMAT   2.0\n"
		"##Type    double\n"
		"##Locale  C\n"
		"##Rows    X\n"
		"##Columns \n"
		"1 .\n"
		"X1 1.00\n"
		"X2 2.00\n"
		"X3 5.00\n"
	);

	EXPECT_EVAL("mdcat(\"x%$0-$1-$2\", a, b, 5).print(cout);",
		"##MDMAT   2.0\n"
		"##Type    int\n"
		"##Locale  C\n"
		"##Rows    x\n"
		"##Columns \n"
		"1 .\n"
		"A-x-1 1\n"
		"B-x-2 2\n"
		"-x-3 5\n"
	);

	EXPECT_EVAL("mdcat(\"x%$0-$1-$2=a,b\", a, b, a, 5).print(cout);",
		"##MDMAT   2.0\n"
		"##Type    int\n"
		"##Locale  C\n"
		"##Rows    x\n"
		"##Columns \n"
		"1 .\n"
		"a 1\n"
		"b 2\n"
		"A-x-3 1\n"
		"-x-4 5\n"
	);

	EXPECT_EVAL("mdcat(\"x%$0-$1-$2=a,b,c,d,e\", a, b, a, 5).print(cout);",
		"##MDMAT   2.0\n"
		"##Type    int\n"
		"##Locale  C\n"
		"##Rows    x\n"
		"##Columns \n"
		"1 .\n"
		"a 1\n"
		"b 2\n"
		"c 1\n"
		"d 5\n"
		"e 0\n"
	);

	PRINT_EVAL("mdmat a = mdcat(\"b=b1\", mdcat(\"a=a1,a2\", 1, 2))");
	PRINT_EVAL("mdmat x = mdcat(\"x=x1,x2,x3\", 10, 20, 30)");
	EXPECT_EVAL("mdcat(\"z=z1,z2,z3,z4\", a, x, 100).mark(\"#-1\").print(cout);",
		"##MDMAT   2.0\n"
		"##Type    int\n"
		"##Locale  C\n"
		"##Rows    z x\n"
		"##Columns a\n"
		"2 a1 a2\n"
		"z1.x1 1 2\n"
		"z1.x2 0 0\n"
		"z1.x3 0 0\n"
		"z2.x1 10 10\n"
		"z2.x2 20 20\n"
		"z2.x3 30 30\n"
		"z3.x1 100 100\n"
		"z3.x2 100 100\n"
		"z3.x3 100 100\n"
		"z4.x1 0 0\n"
		"z4.x2 0 0\n"
		"z4.x3 0 0\n"
	);

	CLEANUP_EVAL;
}
