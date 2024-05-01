#include <EFEU/UnitTest.h>
#include <EFEU/mdmat.h>
#include "utest/md.tst.h"

TEST_SUITE
{
	SetupMdMat();
	mdmat *md = md_create(&Type_int, "name a=a1 b=b1,b2 c=c1 d=d1");
	EXPECT_STR(mdtest_show(md, "hx"),
		"int[2=1*2*1*1] \"name\"\n"
		"a=a1\n"
		"b=b1,b2\n"
		"c=c1\n"
		"d=d1\n"
	);

	TEST_EXPR(md_reduce(md, "[bc]"));
	EXPECT_STR(mdtest_show(md, "hx"),
		"int[2=1*2*1] \"name\"\n"
		"a=a1\n"
		"b=b1,b2\n"
		"d=d1\n"
	);

	TEST_EXPR(md_reduce(md, NULL));
	EXPECT_STR(mdtest_show(md, "hx"),
		"int[2] \"name\"\n"
		"b=b1,b2\n"
	);
	rd_deref(md);
}
