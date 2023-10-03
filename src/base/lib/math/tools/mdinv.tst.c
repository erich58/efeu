#include <EFEU/UnitTest.h>
#include <Math/mdmath.h>
#include "utest/out.tst.h"

TEST_DECL(setup)
{
	SetupStd();
	SetProgName("ID");
	SetApplPath(UnitTest_tmp(NULL));
	TEST_FILE("mdmath.msg",
		"1	No base\n"
		"2	Bad type: $1\n"
		"3	Not square: lines=$1 cols=$2\n"
		"4	Not invertible: dim=$1 range=$2\n"
	);
}

TEST_DECL(no_base)
{
	UnitTest_stderr("stderr");
	EXPECT(mdinv(NULL) == NULL);
	UnitTest_stderr(NULL);
	EXPECT_CONTENT("stderr", "MSG No base\n");
}

TEST_DECL(simple)
{
	const char *data =
		"##MDMAT 2.0\n"
		"##Type double\n"
		"##Locale us\n"
		"##Rows y\n"
		"##Columns x\n"
		"2 x1 x2\n"
		"y1 1 2\n"
		"y2 2 -1\n"
	;
	mdmat *md = md_read(io_cstr(data), NULL);
	md_setflag(md, "#-1", 0, mdsf_mark, MDXFLAG_MARK, NULL, 0); 
	md_print(io_refer(OUT()), md, "mode=data");
	EXPECT_NOUT(md, "1.0000, 2.0000\n2.0000, -1.0000\n");

	mdmat *inv = mdinv(md);
	md_print(io_refer(OUT()), inv, "mode=data");
	EXPECT_NOUT(inv, "0.20000, 0.40000\n0.40000, -0.20000\n");
}

TEST_DECL(no_inv)
{
	const char *data =
		"##MDMAT 2.0\n"
		"##Type double\n"
		"##Locale us\n"
		"##Zeilen y\n"
		"##Spalten x\n"
		"2 x1 x2\n"
		"y1 1 1\n"
		"y2 1 1\n"
	;
	mdmat *md = md_read(io_cstr(data), NULL);
	md_setflag(md, "#-1", 0, mdsf_mark, MDXFLAG_MARK, NULL, 0); 
	md_print(io_refer(OUT()), md, "mode=data");
	EXPECT_NOUT(md, "1.0000, 1.0000\n1.0000, 1.0000\n");
	UnitTest_stderr("stderr");
	EXPECT(mdinv(md) == NULL);
	UnitTest_stderr(NULL);
	EXPECT_CONTENT("stderr", "MSG Not invertible: dim=2 range=1\n");
}

TEST_DECL(not_square)
{
	const char *data =
		"##MDMAT 2.0\n"
		"##Type double\n"
		"##Locale us\n"
		"##Zeilen y\n"
		"##Spalten x\n"
		"2 x1 x2\n"
		"y1 1 1\n"
	;
	mdmat *md = md_read(io_cstr(data), NULL);
	md_setflag(md, "#-1", 0, mdsf_mark, MDXFLAG_MARK, NULL, 0); 
	md_print(io_refer(OUT()), md, "mode=data");
	EXPECT_NOUT(md, "1.0000, 1.0000\n");
	UnitTest_stderr("stderr");
	EXPECT(mdinv(md) == NULL);
	UnitTest_stderr(NULL);
	EXPECT_CONTENT("stderr", "MSG Not square: lines=1 cols=2\n");
}

TEST_DECL(bad_type)
{
	const char *data =
		"##MDMAT 2.0\n"
		"##Type struct { int a; int b }\n"
		"##Locale us\n"
		"##Zeilen y\n"
		"##Spalten x\n"
		"2 x.a x.b\n"
		"y 1 1\n"
	;
	mdmat *md = md_read(io_cstr(data), NULL);
	UnitTest_stderr("stderr");
	EXPECT(mdinv(md) == NULL);
	UnitTest_stderr(NULL);
	EXPECT_CONTENT("stderr", "MSG Bad type: struct { int a, b }\n");
}

TEST_SUITE
{
	TEST_RUN(setup);
	TEST_RUN(no_base);
	TEST_RUN(simple);
	TEST_RUN(no_inv);
	TEST_RUN(not_square);
	TEST_RUN(bad_type);
	CLEANUP_OUT;
}
