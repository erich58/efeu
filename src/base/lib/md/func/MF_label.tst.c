#include <EFEU/UnitTest.h>
#include <EFEU/mdmat.h>
#include <EFEU/utest/eval.tst.h>

TEST_SUITE
{
	SetupMdMat();
	SetProgName("ID");

	TEST_FILE("setup",
		"mdmat a = mdcat(\"t\", 11 : 13)\n"
		"mdmat b = mdcat(\"t\", 21 : 23)\n"
		"mdmat x = mdcat(\"x=a,b\", a, b)\n"
	);

	TEST_EXPR(streval(UnitTest_content("setup")));

	EXPECT_EVAL("mdmat(NULL).label()", "{}");
	EXPECT_EVAL("x.label()", "{}");
	EXPECT_EVAL("x.label(\"x\")", "{\"a\", \"b\"}");
	EXPECT_EVAL("x.label(\"t\")", "{\"t1\", \"t2\", \"t3\"}");
	EXPECT_EVAL("x.label(\"t\", true)", "{\"t=t1\", \"t=t2\", \"t=t3\"}");
	EXPECT_EVAL("x.label(\"x=#1 t=#2\", true)", "{\"x=a\", \"t=t2\"}");
	CLEANUP_EVAL;
}
