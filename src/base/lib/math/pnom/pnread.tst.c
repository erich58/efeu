#include <EFEU/UnitTest.h>
#include <Math/pnom.h>
#include "utest/out.tst.h"
#include <locale.h>

TEST_DECL(setup)
{
	SetupStd();
	SetProgName("ID");
	SetApplPath(UnitTest_tmp(NULL));
	setlocale(LC_ALL, "C");
	TEST_FILE("pnom.msg",
		"11	$!: Bad header\n"
		"12	$!: Bad data line\n"
		"13	$!: Bad coefficients\n"
	);
}

TEST_DECL(syntax)
{
	TEST_IO_DECL(in);

	UnitTest_init(&in, "", 0);
	UnitTest_stderr("stderr");
	TEST_FORK { pnread(UnitTest_file(&in)); }
	CHILD_EXIT(1);
	UnitTest_stderr(NULL);
	EXPECT_CONTENT("stderr", "ERR ID: Bad header\n");

	UnitTest_init(&in, "2 2\n", 0);
	UnitTest_stderr("stderr");
	TEST_FORK { pnread(UnitTest_file(&in)); }
	CHILD_EXIT(1);
	UnitTest_stderr(NULL);
	EXPECT_CONTENT("stderr", "ERR ID: Bad data line\n");

	UnitTest_init(&in, "2 2\n0.0\n", 0);
	UnitTest_stderr("stderr");
	TEST_FORK { pnread(UnitTest_file(&in)); }
	CHILD_EXIT(1);
	UnitTest_stderr(NULL);
	EXPECT_CONTENT("stderr", "ERR ID: Bad coefficients\n");
}

TEST_DECL(right)
{
	TEST_IO_DECL(in);

	UnitTest_init(&in, " 2  3\n\t0 0 1.5 0 -0.5\n 1 1 0 -1.5 0.5\n", 0);
	Polynom *pn = pnread(UnitTest_file(&in));
	pnprint(OUT(), pn, "%.2f");
	EXPECT_OUT(
		"2 3\n"
		"0.00 0.00 1.50 0.00 -0.50\n"
		"1.00 1.00 0.00 -1.50 0.50\n"
	);
}

TEST_SUITE
{
	TEST_RUN(setup);
	TEST_RUN(syntax);
	TEST_RUN(right);
	CLEANUP_OUT;
}
