#include <EFEU/UnitTest.h>
#include <EFEU/mdmat.h>
#include "utest/md.tst.h"
#include "utest/out.tst.h"


TEST_SUITE
{
	SetupMdMat();
	mdmat *md;
	EXPECT_PTR(md = md_create(&Type_int, "name a=a1,a2 b=b1,b2"));
	EXPECT_STR(mdtest_show(md, "hx"),
		"int[4=2*2] \"name\"\n"
		"a=a1,a2\n"
		"b=b1,b2\n"
	);
	EXPECT_INT(md_paste(md, "x", "_", 0, 0), 2);
	EXPECT_STR(mdtest_show(md, "hx"),
		"int[4] \"name\"\n"
		"x=a1_b1,a1_b2,a2_b1,a2_b2\n"
	);
	StrPool_dump(md->sbuf, OUT());
	EXPECT_OUT(
		"  ----- ------------\n"
		"      0 \"StrPool\"\n"
		"  ----- ------------\n"
		"      8 \"name\"\n"
		"     13 \"a\"\n"
		"     15 \"a1\"\n"
		"     18 \"a2\"\n"
		"     21 \"b\"\n"
		"     23 \"b1\"\n"
		"     26 \"b2\"\n"
		"     29 \"x\"\n"
		"     31 \"a1_b1\"\n"
		"     37 \"a1_b2\"\n"
		"     43 \"a2_b1\"\n"
		"     49 \"a2_b2\"\n"
		"  ----- ------------\n"
	);

	TEST_EXPR(md_reorg_label(md));
	EXPECT_STR(mdtest_show(md, "hx"),
		"int[4] \"name\"\n"
		"x=a1_b1,a1_b2,a2_b1,a2_b2\n"
	);
	StrPool_dump(md->sbuf, OUT());
	EXPECT_OUT(
		"  ----- ------------\n"
		"      0 \"StrPool\"\n"
		"  ----- ------------\n"
		"      8 \"name\"\n"
		"     13 \"x\"\n"
		"     15 \"a1_b1\"\n"
		"     21 \"a1_b2\"\n"
		"     27 \"a2_b1\"\n"
		"     33 \"a2_b2\"\n"
		"  ----- ------------\n"
	);
	rd_deref(md);

	CLEANUP_OUT;
}
