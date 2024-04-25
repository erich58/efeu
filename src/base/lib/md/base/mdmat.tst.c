#include <EFEU/UnitTest.h>
#include <EFEU/mdmat.h>
#include "utest/eval.tst.h"

TEST_DECL(index)
{
	EXPECT_EVAL("mdidx i", "NULL");
	EXPECT_EVAL("i.name", "NULL");
	EXPECT_EVAL("i.desc", "NULL");
	EXPECT_EVAL("i.flags", "0");
}

TEST_DECL(null)
{
	EXPECT_EVAL("mdmat n = NULL", "NULL");
	EXPECT_EVAL("n.title", "NULL");
	EXPECT_EVAL("n.desc", "NULL");
	EXPECT_EVAL("n.type", "void");
	EXPECT_EVAL("n.dim", "0");
	EXPECT_EVAL("n.size", "0");
	EXPECT_EVAL("n.axis", "NULL");
	EXPECT_EVAL("n.axis.next", "NULL");
	EXPECT_EVAL("n.axis.name", "NULL");
	EXPECT_EVAL("n.axis.desc", "NULL");
	EXPECT_EVAL("n.axis.dim", "0");
	EXPECT_EVAL("n.axis.flags", "0");
	EXPECT_EVAL("n.axis.index", "{}");
}

TEST_DECL(skalar)
{
	EXPECT_EVAL("mdmat s = 1", "MD: name=NULL, type=int, dim=0, size=4");
	EXPECT_EVAL("s.title = \"Title\"", "\"Title\"");
	EXPECT_EVAL("s.desc = \"Description\"", "\"Description\"");
	EXPECT_EVAL("s.type", "int");
	EXPECT_EVAL("s.dim", "0");
	EXPECT_EVAL("s.size", "4");
	EXPECT_EVAL("s.axis", "NULL");
	EXPECT_EVAL("s.axis.next", "NULL");
	EXPECT_EVAL("s.axis.name", "NULL");
	EXPECT_EVAL("s.axis.desc", "NULL");
	EXPECT_EVAL("s.axis.dim", "0");
	EXPECT_EVAL("s.axis.flags", "0");
	EXPECT_EVAL("s.axis.index", "{}");
}

TEST_DECL(matrix)
{
	TEST_FILE("a",
		"##MDMAT   2.0\n"
		"##Title   Title\n"
		"##Desc    Description\n"
		"##Type    int\n"
		"##Locale  C\n"
		"##Rows    r\n"
		"##Columns c\n"
		"2 x y z\n"
		"a 1 2 3\n"
		"b 7 0 5\n"
	);
	EXPECT_EVAL(UnitTest_tpl("str tmp = \"@\""),
		UnitTest_qstr(UnitTest_tmp(NULL)));
	EXPECT_EVAL("mdmat a = mdread(findopen(tmp, \"a\"))",
		"MD: name=\"Title\", type=int, dim=2, size=24");
	EXPECT_EVAL("a.show(cout)",
		"int[6=2*3] \"Title\"\n"
		"r=a,b\n"
		"c=x,y,z\n"
	);
	EXPECT_EVAL("a.title", "\"Title\"");
	EXPECT_EVAL("a.desc", "\"Description\"");
	EXPECT_EVAL("a.type", "int");
	EXPECT_EVAL("a.dim", "2");
	EXPECT_EVAL("a.size", "24");

	EXPECT_EVAL("a.axis",
		"(name = \"r\", dim = 2, size = 12, flags = 0)");
	EXPECT_EVAL("a.axis.next",
		"(name = \"c\", dim = 3, size = 4, flags = 0)");
	EXPECT_EVAL("a.axis.next.next", "NULL");

	EXPECT_EVAL("a.axis[0]",
		"(name = \"r\", dim = 2, size = 12, flags = 0)");
	EXPECT_EVAL("a.axis[1]",
		"(name = \"c\", dim = 3, size = 4, flags = 0)");
	EXPECT_EVAL("a.axis[2]", "NULL");

	EXPECT_EVAL("a.axis[\"r\"]",
		"(name = \"r\", dim = 2, size = 12, flags = 0)");
	EXPECT_EVAL("a.axis[\"c\"]",
		"(name = \"c\", dim = 3, size = 4, flags = 0)");
	EXPECT_EVAL("a.axis[\"x\"]", "NULL");

	EXPECT_EVAL("a.axis.name", "\"r\"");
	EXPECT_EVAL("a.axis.desc", "NULL");
	EXPECT_EVAL("a.axis.index", "{(\"a\", 0), (\"b\", 0)}");
	EXPECT_EVAL("a.axis.index.obj", "(\"a\", 0)");
	EXPECT_EVAL("a.axis.index.obj.name", "\"a\"");
	EXPECT_EVAL("a.axis.index.obj.desc", "NULL");
	EXPECT_EVAL("a.axis.index.obj.flags", "0");
}

TEST_SUITE
{
	SetupMdMat();
	PRINT_EVAL("fmt_float = \"%#*.*f\"");
	PRINT_EVAL("float_prec = 2");
	TEST_RUN(index);
	TEST_RUN(null);
	TEST_RUN(skalar);
	TEST_RUN(matrix);
	CLEANUP_EVAL;
}

