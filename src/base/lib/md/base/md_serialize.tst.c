
#include <EFEU/UnitTest.h>
#include <EFEU/mdmat.h>
#include "utest/md.tst.h"

TEST_SUITE
{
	mdmat *md;
	SetupMdMat();

	EXPECT_INT(md_serialize(NULL), 0);

	EXPECT_PTR(md = mdtest_create("a=a1,a2 b=b1 c=c1,c2,c3"));
	EXPECT_STR(mdtest_show(md, "sd"),
		"int          24       6       4\n"
		"a            24       2      12\n"
		"b            12       1      12\n"
		"c            12       3       4\n"
		"a       b       c           # int[6]\n"
		"a1      b1      c1          0 111\n"
		"a1      b1      c2          1 112\n"
		"a1      b1      c3          2 113\n"
		"a2      b1      c1          3 211\n"
		"a2      b1      c2          4 212\n"
		"a2      b1      c3          5 213\n"
	);
	EXPECT_INT(md_serialize(md), 0);

	TEST_EXPR(md_permut(md, "c,a,b"));
	EXPECT_STR(mdtest_show(md, "sd"),
		"int          24       6       4\n"
		"c            12       3       4\n"
		"a            24       2      12\n"
		"b            12       1      12\n"
		"c       a       b           # int[6]\n"
		"c1      a1      b1          0 111\n"
		"c1      a2      b1          3 211\n"
		"c2      a1      b1          1 112\n"
		"c2      a2      b1          4 212\n"
		"c3      a1      b1          2 113\n"
		"c3      a2      b1          5 213\n"
	);

	EXPECT_INT(md_serialize(md), 1);
	EXPECT_STR(mdtest_show(md, "sd"),
		"int          24       6       4\n"
		"c            24       3       8\n"
		"a             8       2       4\n"
		"b             4       1       4\n"
		"c       a       b           # int[6]\n"
		"c1      a1      b1          0 111\n"
		"c1      a2      b1          1 211\n"
		"c2      a1      b1          2 112\n"
		"c2      a2      b1          3 212\n"
		"c3      a1      b1          4 113\n"
		"c3      a2      b1          5 213\n"
	);

	rd_deref(md);
}
