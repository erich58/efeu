#include <EFEU/UnitTest.h>
#include <EFEU/mdmat.h>
#include "utest/eval.tst.h"

TEST_SUITE
{
	SetupMdMat();
	PRINT_EVAL("fmt_float = \"%#*.*f\"");
	PRINT_EVAL("float_prec = 2");

	EXPECT_EVAL("mdmat x = NULL", "NULL");
	EXPECT_EVAL("x += x", "NULL");
	EXPECT_EVAL("x + x", "NULL");
	EXPECT_EVAL("x.expr(\"+\", x)", "NULL");
	EXPECT_EVAL("x.expr(\"++\")", "NULL");
	EXPECT_EVAL("mdexpr(operator\"+\", x, x)", "NULL");
	EXPECT_EVAL("mdexpr(operator\"++\", x)", "NULL");
	EXPECT_EVAL("x.round()", "NULL");
	EXPECT_EVAL("x.adjust()", "NULL");
	EXPECT_EVAL("x.reduce()", "NULL");
	EXPECT_EVAL("(EDB) x", "NULL");
	EXPECT_EVAL("x.mark()", "NULL");
	EXPECT_EVAL("x.transp()", "NULL");
	EXPECT_EVAL("x.time()", "NULL");
	EXPECT_EVAL("x.hide()", "NULL");
	EXPECT_EVAL("x.select()", "NULL");
	EXPECT_EVAL("x.select(\"x\")", "NULL");
	EXPECT_EVAL("x.base()", "NULL");
	EXPECT_EVAL("x.base(\"x\")", "NULL");
	EXPECT_EVAL("x.permut(NULL)", "NULL");

	TEST_FILE("a",
		"##MDMAT   2.0\n"
		"##Title    Data\n"
		"##Type    int\n"
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
		"MD: int[4=2*2] \"Data\"\n");
	EXPECT_EVAL("a.show(cout)",
		"int[4=2*2] \"Data\"\n"
		"r=a,b\n"
		"c=x,y\n"
	);

	PRINT_EVAL("a.save(tmp + \"/b\")");
	PRINT_EVAL("a.map(tmp + \"/c\")");
	EXPECT_EVAL("mdfile(cout, tmp + \"/b\");",
		UnitTest_tpl("@/b: int[4=2*2] \"Data\"\n"));
	EXPECT_EVAL("mdfile(cout, tmp + \"/c\");",
		UnitTest_tpl("@/c: int[4=2*2] \"Data\"\n"));

	EXPECT_EVAL("mdmat b = mdload(tmp + \"/b\")",
		"MD: int[4=2*2] \"Data\"\n");
	EXPECT_EVAL("b.mark(\"c\").print(cout, \"nohead\");",
		"2 x y\n"
		"a 1 2\n"
		"b 3 0\n"
	);
	EXPECT_EVAL("mdmat c = reload(b, \"c=x\")",
		"MD: int[2=2*1] \"Data\"\n");
	EXPECT_EVAL("c.mark(\"c\").print(cout, \"nohead\");",
		"1 x\n"
		"a 1\n"
		"b 3\n"
	);

	EXPECT_EVAL("mdmat(a).mark(\"c\").print(cout, \"nohead\");",
		"2 x y\n"
		"a 1 2\n"
		"b 3 0\n"
	);
	EXPECT_EVAL("mdmat(a, \"c=x\").mark(\"c\").print(cout, \"nohead\");",
		"1 x\n"
		"a 1\n"
		"b 3\n"
	);

	EXPECT_EVAL("mdmat(42).print(cout, \"nohead\");",
		"1 .\n"
		". 42\n"
	);

	EXPECT_EVAL("mdmat(int, \"x=a,b\").print(cout, \"nohead\");",
		"1 .\n"
		"a 0\n"
		"b 0\n"
	);

	PRINT_EVAL("x = NULL");
	EXPECT_EVAL("x.paste(\"c\", 0, 0)", "NULL");
	PRINT_EVAL("x = mdmat(a)");
	EXPECT_EVAL("x.paste(\"z\", 0, 0, \"-\")", "MD: int[4]\n");
	EXPECT_EVAL("x.print(cout, \"nohead\");",
		"1 .\n"
		"a-x 1\n"
		"a-y 2\n"
		"b-x 3\n"
		"b-y 0\n"
	);

	PRINT_EVAL("x = mdmat(a)");
	PRINT_EVAL("x.mark(\"c\")");
	EXPECT_EVAL("mdmul(x, x).print(cout, \"nohead\");",
		"2 a b\n"
		"a 5 3\n"
		"b 3 9\n"
	);

	PRINT_EVAL("x = mdmat(a)");
	PRINT_EVAL("x.mark(\"r\")");
	EXPECT_EVAL("mdprod(x).print(cout, \"nohead\");",
		"2 a b\n"
		"a 5 3\n"
		"b 3 9\n"
	);

	PRINT_EVAL("x = mdmat(a)");
	PRINT_EVAL("x.conv(double)");
	EXPECT_EVAL("x.mark(\"c\").print(cout, \"nohead\");",
		"2 x y\n"
		"a 1.00 2.00\n"
		"b 3.00 0.00\n"
	);

	EXPECT_EVAL("a[0]", "1");
	EXPECT_EVAL("a[5]", "");

	EXPECT_EVAL("(EfiVec) a", "{1, 2, 3, 0}");
	EXPECT_EVAL("mdcat(NULL)", "NULL");
	EXPECT_EVAL("mdcat(\"z\", a, a).mark(\"#-1\").print(cout,\"nohead\");",
		"2 x y\n"
		"z1.a 1 2\n"
		"z1.b 3 0\n"
		"z2.a 1 2\n"
		"z2.b 3 0\n"
	);

	EXPECT_EVAL("sumaxis(a, \"r\").print(cout,\"nohead\");",
		"1 .\n"
		"x 4\n"
		"y 2\n"
	);
	EXPECT_EVAL("mddiag(a, \"c\").mark(\"#-1\").print(cout,\"nohead\");",
		"2 x y\n"
		"a.x 1 0\n"
		"a.y 0 2\n"
		"b.x 3 0\n"
		"b.y 0 0\n"
	);

	EXPECT_EVAL("a.axis[1]",
		"(name = \"c\", dim = 2, size = 4, flags = 0)");
	EXPECT_EVAL("a.axis[\"r\"]",
		"(name = \"r\", dim = 2, size = 8, flags = 0)");
	EXPECT_EVAL("a.axis.choice(\"r\")", "");
	EXPECT_EVAL("a.axis.choice(0)", "");

	CLEANUP_EVAL;
}
