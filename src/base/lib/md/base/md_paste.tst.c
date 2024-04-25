#include <EFEU/UnitTest.h>
#include <EFEU/mdmat.h>
#include "utest/md.tst.h"

TEST_DECL(last)
{
	mdmat *md;
	EXPECT_PTR(md = mdtest_create("a=a1,a2 b=b1,b2 c=c1,c2"));
	EXPECT_STR(mdtest_show(md, "s"),
		" 32   8   4 int[8=2*2*2]\n"
		" 32   2  16 a=a1,a2\n"
		" 16   2   8 b=b1,b2\n"
		"  8   2   4 c=c1,c2\n"
	);
	EXPECT_INT(md_paste(md, "x", ".", -2, 0), 2);
	EXPECT_STR(mdtest_show(md, "sd"),
		" 32   8   4 int[8=2*4]\n"
		" 32   2  16 a=a1,a2\n"
		" 16   4   4 x=b1.c1,b1.c2,b2.c1,b2.c2\n"
		"a       x           # int[8]\n"
		"a1      b1.c1       0 111\n"
		"a1      b1.c2       1 112\n"
		"a1      b2.c1       2 121\n"
		"a1      b2.c2       3 122\n"
		"a2      b1.c1       4 211\n"
		"a2      b1.c2       5 212\n"
		"a2      b2.c1       6 221\n"
		"a2      b2.c2       7 222\n"
	); 
	rd_deref(md);
}

TEST_DECL(unnamed)
{
	mdmat *md;

	EXPECT_PTR(md = mdtest_create("a=a1,a2 b=b1,b2"));
	EXPECT_STR(mdtest_show(md, "sd"),
		" 16   4   4 int[4=2*2]\n"
		" 16   2   8 a=a1,a2\n"
		"  8   2   4 b=b1,b2\n"
		"a       b           # int[4]\n"
		"a1      b1          0 11\n"
		"a1      b2          1 12\n"
		"a2      b1          2 21\n"
		"a2      b2          3 22\n"
	); 
	EXPECT_INT(md_paste(md, NULL, ".", 0, 0), 2);
	EXPECT_STR(mdtest_show(md, "sd"),
		" 16   4   4 int[4=4]\n"
		" 16   4   4 a.b=a1.b1,a1.b2,a2.b1,a2.b2\n"
		"a.b         # int[4]\n"
		"a1.b1       0 11\n"
		"a1.b2       1 12\n"
		"a2.b1       2 21\n"
		"a2.b2       3 22\n"
	); 

	rd_deref(md);
}

TEST_DECL(permut)
{
	mdmat *md;
	EXPECT_PTR(md = mdtest_create("a=a1,a2 b=b1,b2 c=c1,c2"));

	TEST_EXPR(md_permut(md, "c"));
	EXPECT_STR(mdtest_show(md, "sd"),
		" 32   8   4 int[8=2*2*2]\n"
		"  8   2   4 c=c1,c2\n"
		" 32   2  16 a=a1,a2\n"
		" 16   2   8 b=b1,b2\n"
		"c       a       b           # int[8]\n"
		"c1      a1      b1          0 111\n"
		"c1      a1      b2          2 121\n"
		"c1      a2      b1          4 211\n"
		"c1      a2      b2          6 221\n"
		"c2      a1      b1          1 112\n"
		"c2      a1      b2          3 122\n"
		"c2      a2      b1          5 212\n"
		"c2      a2      b2          7 222\n"
	);
	EXPECT_INT(md_paste(md, "x", ".", 1, 2), 2);
	EXPECT_STR(mdtest_show(md, "sd"),
		" 32   8   4 int[8=2*4]\n"
		"  8   2   4 c=c1,c2\n"
		" 32   4   8 x=a1.b1,a1.b2,a2.b1,a2.b2\n"
		"c       x           # int[8]\n"
		"c1      a1.b1       0 111\n"
		"c1      a1.b2       2 121\n"
		"c1      a2.b1       4 211\n"
		"c1      a2.b2       6 221\n"
		"c2      a1.b1       1 112\n"
		"c2      a1.b2       3 122\n"
		"c2      a2.b1       5 212\n"
		"c2      a2.b2       7 222\n"
	);

	rd_deref(md);
}

TEST_DECL(permut_serialized)
{
	mdmat *md;
	EXPECT_PTR(md = mdtest_create("a=a1 b=b1,b2 c=c1,c2 d=d2"));

	TEST_EXPR(md_permut(md, "a d c"));
	EXPECT_STR(mdtest_show(md, "sd"),
		" 16   4   4 int[4=1*1*2*2]\n"
		" 16   1  16 a=a1\n"
		"  4   1   4 d=d2\n"
		"  8   2   4 c=c1,c2\n"
		" 16   2   8 b=b1,b2\n"
		"a       d       c       b           # int[4]\n"
		"a1      d2      c1      b1          0 1111\n"
		"a1      d2      c1      b2          2 1211\n"
		"a1      d2      c2      b1          1 1121\n"
		"a1      d2      c2      b2          3 1221\n"
	);
	EXPECT_INT(md_paste(md, "x", ".", 1, 0), 3);
	EXPECT_STR(mdtest_show(md, "sd"),
		" 16   4   4 int[4=1*4]\n"
		" 16   1  16 a=a1\n"
		" 16   4   4 x=d2.c1.b1,d2.c1.b2,d2.c2.b1,d2.c2.b2\n"
		"a       x           # int[4]\n"
		"a1      d2.c1.b1            0 1111\n"
		"a1      d2.c1.b2            1 1211\n"
		"a1      d2.c2.b1            2 1121\n"
		"a1      d2.c2.b2            3 1221\n"
	);

	rd_deref(md);
}

TEST_SUITE
{
	SetupMdMat();
	EXPECT_INT(md_paste(NULL, "x", ".", 0, 1), 0);
	TEST_RUN(last);
	TEST_RUN(unnamed);
	TEST_RUN(permut);
	TEST_RUN(permut_serialized);
}
