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

	rd_deref(md);
}
