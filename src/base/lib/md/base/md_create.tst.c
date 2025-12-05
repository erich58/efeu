#include <EFEU/UnitTest.h>
#include <EFEU/mdmat.h>
#include <EFEU/utest/out.tst.h>

static const char *utest_md(mdmat *md)
{
	md_print(io_refer(OUT()), md, "head");
	rd_deref(md);
	return utest_out();
}

#define	EXPECT_MD(m, s)	UnitTest_strcheck(__FILE__, __LINE__, \
	"==", utest_md(m), s, # m, # s)


TEST_SUITE
{
	SetupMdMat();

	EXPECT_MD(md_create(NULL, NULL), "");

	EXPECT_MD(md_create(&Type_int, NULL),
		"##MDMAT   2.0\n"
		"##Type    int\n"
		"##Locale  C\n"
		"##Rows    \n"
		"##Columns \n"
		"1 .\n"
		". 0\n"
	);

	EXPECT_MD(md_create(&Type_int, "title a=a1,a2 b x=x,y"),
		"##MDMAT   2.0\n"
		"##Title   title\n"
		"##Type    int\n"
		"##Locale  C\n"
		"##Rows    a b x\n"
		"##Columns \n"
		"1 .\n"
		"a1.b.x 0\n"
		"a1.b.y 0\n"
		"a2.b.x 0\n"
		"a2.b.y 0\n"
	);

	CLEANUP_OUT;
}
