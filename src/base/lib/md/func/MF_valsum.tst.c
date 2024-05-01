#include <EFEU/UnitTest.h>
#include <EFEU/mdmat.h>
#include "utest/eval.tst.h"

TEST_SUITE
{
	SetupMdMat();
	SetProgName("ID");
	SetApplPath(UnitTest_tmp(NULL));
	TEST_FILE("mdmat.msg",
		"16	$!: can not add values of type '$1'.\n"
	);
	PRINT_EVAL("fmt_float = \"%#*.*f\"");
	PRINT_EVAL("float_prec = 2");

	EXPECT_EVAL("sum(mdcat(NULL, 1.7, 3.4))", "5.10");
	EXPECT_EVAL("sum(mdcat(NULL, 2, 7))", "9");
	UnitTest_stderr("stderr");
	EXPECT_EVAL("sum(mdmat(void))", "");
	UnitTest_stderr(NULL);
	EXPECT_CONTENT("stderr",
		"MSG ID: can not add values of type 'void'.\n");

	EXPECT_EVAL("mdmat md", "NULL");
	EXPECT_EVAL("sum(md)", "");

	TEST_EXPR(streval("md = mdcat(\"x\", 1 : 5);"));
	PRINT_EVAL("md.show(cout, \"d\");");
	EXPECT_EVAL("sum(md)", "15");
	TEST_EXPR(streval("md.select(\"x=#3:\");"));
	PRINT_EVAL("md.show(cout, \"d\");");
	EXPECT_EVAL("sum(md)", "12");
	EXPECT_EVAL("sum(md, \"x=x1\")", "0");
	EXPECT_EVAL("sum(md, \"x=x5\")", "5");
	EXPECT_EVAL("sum(md, \"x=x1,x2,x3\")", "3");
	EXPECT_EVAL("sum(md, \"x=x4,x5\")", "9");

	CLEANUP_EVAL;
}
