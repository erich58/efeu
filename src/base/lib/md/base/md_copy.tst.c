#include <EFEU/UnitTest.h>
#include <EFEU/mdmat.h>
#include "utest/eval.tst.h"

static const char *utest_md(mdmat *md)
{
	md_mark(md, "#-1");
	md_print(io_refer(OUT()), md, NULL);
	rd_deref(md);
	return utest_out();
}

#define	EXPECT_MD(m, s)	UnitTest_strcheck(__FILE__, __LINE__, \
	"==", utest_md(m), s, # m, # s)


TEST_SUITE
{
	mdmat *md;

	SetupMdMat();
	PRINT_EVAL("fmt_float = \"%#*.*f\"");
	PRINT_EVAL("float_prec = 2");

	EXPECT(md_copy(NULL, NULL) == NULL);

	EXPECT_PTR(md = md_create(&Type_double, "test a=a,b,c,d x=x,y,z"));

	double *data = md->data;
	size_t n = md->axis->dim;
	size_t k = md->axis->next->dim;

	for (size_t i = 0; i < n; ++i)
		for (size_t j = 0; j < k; ++j)
			data[k * i + j] = i + 1. + 0.1 * (j + 1.);

	EXPECT_MD(rd_refer(md),
		"##MDMAT   2.0\n"
		"##Title   test\n"
		"##Type    double\n"
		"##Locale  C\n"
		"##Rows    a\n"
		"##Columns x\n"
		"3 x y z\n"
		"a 1.10 1.20 1.30\n"
		"b 2.10 2.20 2.30\n"
		"c 3.10 3.20 3.30\n"
		"d 4.10 4.20 4.30\n"
	);

	EXPECT_MD(md_copy(md, NULL),
		"##MDMAT   2.0\n"
		"##Type    double\n"
		"##Locale  C\n"
		"##Rows    a\n"
		"##Columns x\n"
		"3 x y z\n"
		"a 1.10 1.20 1.30\n"
		"b 2.10 2.20 2.30\n"
		"c 3.10 3.20 3.30\n"
		"d 4.10 4.20 4.30\n"
	);

	EXPECT_MD(md_copy(md, "a=[bc] x=#2:"),
		"##MDMAT   2.0\n"
		"##Type    double\n"
		"##Locale  C\n"
		"##Rows    a\n"
		"##Columns x\n"
		"2 y z\n"
		"b 2.20 2.30\n"
		"c 3.20 3.30\n"
	);

	CLEANUP_EVAL;
}
