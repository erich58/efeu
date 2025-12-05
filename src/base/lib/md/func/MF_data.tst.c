#include <EFEU/UnitTest.h>
#include <EFEU/mdmat.h>
#include <EFEU/utest/eval.tst.h>

TEST_SUITE
{
	SetupMdMat();
	SetProgName("ID");
	SetApplPath(UnitTest_tmp(NULL));
	TEST_FILE("mdmat.msg",
		"95	$!: function $1, type $2, dim $3.\n"
	);

	TEST_FILE("setup",
		"int f0() { return 0; }\n"
		"int f1(int x) { return x; }\n"
		"int f2(int x, int b) { return x - b; }\n"
		"mdmat a = mdcat(\"t\", 11 : 13)\n"
		"mdmat b = mdcat(\"t\", 21 : 23)\n"
		"mdmat x = mdcat(\"x\", a, b)\n"
	);

	TEST_EXPR(streval(UnitTest_content("setup")));

	PRINT_EVAL("x.base(\"x=x1 t=t1\");");
	PRINT_EVAL("x.time(\"t\");");
	EXPECT_EVAL("mdmat(NULL).data(NULL, NULL)", "{}");
	EXPECT_EVAL("x.data(NULL, NULL)", "{11, 12, 13, 21, 22, 23}");
	EXPECT_EVAL("x.data(NULL, f1)", "{11, 12, 13, 21, 22, 23}");
	EXPECT_EVAL("x.data(\"t=#2\", f1)", "{12, 22}");
	EXPECT_EVAL("x.data(NULL, f2)", "{0, 1, 2, 10, 11, 12}");
	EXPECT_EVAL("x.data(NULL, f2, 1)", "{NULL, 1, 1, NULL, 1, 1}");
	UnitTest_stderr("error");
	EXPECT_EVAL("x.data(NULL, f0)", "{}");
	EXPECT_EVAL("x.data(NULL, f1, 1)", "{}");
	UnitTest_stderr(NULL);
	EXPECT_CONTENT("error",
		"MSG ID: function f0, type int, dim 1.\n"
		"MSG ID: function f1, type int, dim 2.\n"
	);

	CLEANUP_EVAL;
}
