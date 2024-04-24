#include <EFEU/UnitTest.h>
#include <EFEU/mdmat.h>
#include "utest/md.tst.h"

TEST_SUITE
{
	mdmat *md;
	SetupMdMat();
	EXPECT_PTR(md = mdtest_create("a=a1,a2 b=b1 c=c1,c2,c3"));
	EXPECT_STR(mdtest_show(md, "d"),
		"a       b       c           # int[6]\n"
		"a1      b1      c1          0 111\n"
		"a1      b1      c2          1 112\n"
		"a1      b1      c3          2 113\n"
		"a2      b1      c1          3 211\n"
		"a2      b1      c2          4 212\n"
		"a2      b1      c3          5 213\n"
	);

	TEST_EXPR(md_permut(md, "c,a,b"));
	EXPECT_STR(mdtest_show(md, "d"),
		"c       a       b           # int[6]\n"
		"c1      a1      b1          0 111\n"
		"c1      a2      b1          3 211\n"
		"c2      a1      b1          1 112\n"
		"c2      a2      b1          4 212\n"
		"c3      a1      b1          2 113\n"
		"c3      a2      b1          5 213\n"
	);

	rd_deref(md);
}
