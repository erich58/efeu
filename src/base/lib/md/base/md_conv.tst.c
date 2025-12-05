#include <EFEU/UnitTest.h>
#include <EFEU/mdmat.h>
#include <EFEU/utest/eval.tst.h>

TEST_SUITE
{
	mdmat *md;

	SetupMdMat();
	SetProgName("ID");
	SetApplPath(UnitTest_tmp(NULL));
	TEST_FILE("mdmat.msg",
		"102	$!: can not convert type $1 in $2.\n"
	);
	PRINT_EVAL("fmt_float = \"%#*.*f\"");
	PRINT_EVAL("float_prec = 2");

	EXPECT_INT(md_conv(NULL, &Type_double), EOF);

	EXPECT_PTR(md = md_create(&Type_int, "test a=a,b,c,d x=x,y,z"));

	int *data = md->data;
	size_t n = md->axis->dim;
	size_t k = md->axis->next->dim;

	for (size_t i = 0; i < n; ++i)
		for (size_t j = 0; j < k; ++j)
			data[k * i + j] = 10 * (i + 1) + j + 1;

	md_mark(md, "#-1");
	md_print(io_refer(OUT()), md, NULL);
	EXPECT_NOUT(md,
		"##MDMAT   2.0\n"
		"##Title   test\n"
		"##Type    int\n"
		"##Locale  C\n"
		"##Rows    a\n"
		"##Columns x\n"
		"3 x y z\n"
		"a 11 12 13\n"
		"b 21 22 23\n"
		"c 31 32 33\n"
		"d 41 42 43\n"
	);

	UnitTest_stderr("stderr");
	EXPECT_INT(md_conv(md, &Type_ptr), EOF);
	UnitTest_stderr(NULL);
	EXPECT_CONTENT("stderr",
		"MSG ID: can not convert type int in _Ptr_.\n");

	EXPECT_INT(md_conv(md, NULL), 0);
	EXPECT_INT(md_conv(md, &Type_int), 0);
	EXPECT_INT(md_conv(md, &Type_double), 1);
	md_print(io_refer(OUT()), md, NULL);
	EXPECT_NOUT(md,
		"##MDMAT   2.0\n"
		"##Title   test\n"
		"##Type    double\n"
		"##Locale  C\n"
		"##Rows    a\n"
		"##Columns x\n"
		"3 x y z\n"
		"a 11.00 12.00 13.00\n"
		"b 21.00 22.00 23.00\n"
		"c 31.00 32.00 33.00\n"
		"d 41.00 42.00 43.00\n"
	);

	rd_deref(md);
	CLEANUP_EVAL;
}
