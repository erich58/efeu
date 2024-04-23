#include <EFEU/UnitTest.h>
#include <EFEU/mdmat.h>
#include "utest/md.tst.h"

TEST_SUITE
{
	mdmat *md;
	SetupMdMat();

	EXPECT_PTR(md = mdtest_create("a=a1,a2 b=b1,b2 c=c1,c2"));
	PRINT_STR(mdtest_show(md));
	rd_deref(md);
}
