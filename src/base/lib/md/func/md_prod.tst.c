#include <EFEU/UnitTest.h>
#include <EFEU/mdmat.h>
#include <EFEU/utest/eval.tst.h>

TEST_SUITE
{
	SetupMdMat();
	SetProgName("ID");
	SetApplPath(UnitTest_tmp(NULL));
	TEST_FILE("mdmat.msg",
		"64	$!: mdmul: can not multiply $1 with $2.\n"
	);

	EXPECT_EVAL("mdprod(NULL)", "NULL");

	UnitTest_stderr("stderr");
	EXPECT_EVAL("mdprod(\"a\")", "NULL");
	UnitTest_stderr(NULL);
	EXPECT_CONTENT("stderr", 
		"MSG ID: mdmul: can not multiply str with str.\n");

	EXPECT_EVAL("mdprod(2).print(cout, \"nohead\");",
		"1 .\n"
		". 4\n"
	);

	PRINT_EVAL("mdmat md = mdcat(\"b\", mdcat(\"a\", 1, 2, 0))");
	EXPECT_EVAL("md.print(cout, \"nohead\");",
		"1 .\n"
		"b1.a1 1\n"
		"b1.a2 2\n"
		"b1.a3 0\n"
	);
	EXPECT_EVAL("mdprod(md).print(cout, \"nohead\");",
		"1 .\n"
		". 5\n"
	);

	EXPECT_EVAL("md.mark(\"b\").print(cout, \"nohead\");",
		"1 b1\n"
		"a1 1\n"
		"a2 2\n"
		"a3 0\n"
	);
	EXPECT_EVAL("mdprod(md).print(cout, \"nohead\");",
		"1 b1\n"
		"b1 5\n"
	);

	EXPECT_EVAL("md.mark(\"a\").print(cout, \"nohead\");",
		"3 a1 a2 a3\n"
		"b1 1 2 0\n"
	);
	EXPECT_EVAL("mdprod(md).print(cout, \"nohead\");",
		"3 a1 a2 a3\n"
		"a1 1 2 0\n"
		"a2 2 4 0\n"
		"a3 0 0 0\n"
	);

	EXPECT_EVAL("md.mark(\"*\").print(cout, \"nohead\");",
		"3 b1.a1 b1.a2 b1.a3\n"
		". 1 2 0\n"
	);
	EXPECT_EVAL("mdprod(md).print(cout, \"nohead\");",
		"3 b1.a1 b1.a2 b1.a3\n"
		"b1.a1 1 2 0\n"
		"b1.a2 2 4 0\n"
		"b1.a3 0 0 0\n"
	);

	CLEANUP_EVAL;
}
