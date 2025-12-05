#include <EFEU/UnitTest.h>
#include <EFEU/mdmat.h>
#include <EFEU/utest/eval.tst.h>

TEST_SUITE
{
	SetupMdMat();
	SetProgName("ID");
	SetApplPath(UnitTest_tmp(NULL));
	TEST_FILE("mdmat.msg",
		"61	$!: mdmul: empty data cube(s).\n"
		"62	$!: mdmul: axis '$1' - '$2' incompatible (flags=$3).\n"
		"63	$!: mdmul: incompatible number of axis.\n"
		"64	$!: mdmul: can not multiply $1 with $2.\n"
	);
	PRINT_EVAL("fmt_float = \"%#*.*f\"");
	PRINT_EVAL("float_prec = 2");

	UnitTest_stderr("stderr");
	EXPECT_EVAL("mdmul(NULL, NULL)", "NULL");
	UnitTest_stderr(NULL);
	EXPECT_CONTENT("stderr", 
		"MSG ID: mdmul: empty data cube(s).\n");

	UnitTest_stderr("stderr");
	EXPECT_EVAL("mdmul(\"a\", \"b\")", "NULL");
	UnitTest_stderr(NULL);
	EXPECT_CONTENT("stderr", 
		"MSG ID: mdmul: can not multiply str with str.\n");


	EXPECT_EVAL("mdmul(2, 3.).print(cout, \"nohead\");",
		"1 .\n"
		". 6.00\n"
	);

	PRINT_EVAL("mdmat a = mdcat(\"c\", mdcat(\"a\", 1, 2, 3)))");
	PRINT_EVAL("mdmat b = mdcat(\"d\", mdcat(\"b\", 4, 5)))");
	PRINT_EVAL("mdmat b2 = mdcat(\"d\", mdcat(\"b\", 4, 5, 6)))");

	EXPECT_EVAL("mdmul(a, b).print(cout, \"nohead\");",
		"3 c1.a1 c1.a2 c1.a3\n"
		"d1.b1 4 8 12\n"
		"d1.b2 5 10 15\n"
	);

	EXPECT_EVAL("mdmul(a, b2).print(cout, \"nohead\");",
		"3 c1.a1 c1.a2 c1.a3\n"
		"d1.b1 4 8 12\n"
		"d1.b2 5 10 15\n"
		"d1.b3 6 12 18\n"
	);

	EXPECT_EVAL("a.mark(\"a\").print(cout, \"nohead\");",
		"3 a1 a2 a3\n"
		"c1 1 2 3\n"
	);
	EXPECT_EVAL("b.mark(\"b\").print(cout, \"nohead\");",
		"2 b1 b2\n"
		"d1 4 5\n"
	);

	UnitTest_stderr("stderr");
	EXPECT_EVAL("mdmul(a, b).print(cout, \"nohead\");",
		"1 c1\n"
		"d1 14\n"
	);
	UnitTest_stderr(NULL);
	EXPECT_CONTENT("stderr", 
		"MSG ID: mdmul: axis 'a' - 'b' incompatible (flags=2).\n");

	UnitTest_stderr("stderr");
	EXPECT_EVAL("mdmul(a, b2)", "NULL");
	UnitTest_stderr(NULL);
	EXPECT_CONTENT("stderr", 
		"MSG ID: mdmul: incompatible number of axis.\n");

	EXPECT_EVAL("b2.mark(\"b\").print(cout, \"nohead\");",
		"3 b1 b2 b3\n"
		"d1 4 5 6\n"
	);

	EXPECT_EVAL("mdmul(a, b2).print(cout, \"nohead\");",
		"1 c1\n"
		"d1 32\n"
	);

	EXPECT_EVAL("a.mark(\"c\").print(cout, \"nohead\");",
		"1 c1\n"
		"a1 1\n"
		"a2 2\n"
		"a3 3\n"
	);

	EXPECT_EVAL("b2.mark(\"d\").print(cout, \"nohead\");",
		"1 d1\n"
		"b1 4\n"
		"b2 5\n"
		"b3 6\n"
	);

	EXPECT_EVAL("mdmul(a, b2).print(cout, \"nohead\");",
		"3 a1 a2 a3\n"
		"b1 4 8 12\n"
		"b2 5 10 15\n"
		"b3 6 12 18\n"
	);

	EXPECT_EVAL("a.mark(\"*\").print(cout, \"nohead\");",
		"3 c1.a1 c1.a2 c1.a3\n"
		". 1 2 3\n"
	);

	EXPECT_EVAL("b2.mark(\"*\").print(cout, \"nohead\");",
		"3 d1.b1 d1.b2 d1.b3\n"
		". 4 5 6\n"
	);

	EXPECT_EVAL("mdmul(a, b2).print(cout, \"nohead\");",
		"1 .\n"
		". 32\n"
	);

	CLEANUP_EVAL;
}
