#include <EFEU/UnitTest.h>
#include <EFEU/mdmat.h>
#include "utest/md.tst.h"

TEST_SUITE
{
	mdmat *md;
	SetupMdMat();

	EXPECT_PTR(md = mdtest_create("name a=a1,a2 b=b1,b2 c=c1,c2"));

	md_mark(md, "b");
	md_select(md, "b=b2");
	md_base(md, "a=a1");

	EXPECT_STR(mdtest_show(md, NULL),
		"int[8=2*2*2] \"name\"\n"
		"a=a1[B],a2\n"
		"b[m]=b1[L],b2\n"
		"c=c1,c2\n"
	);

	EXPECT_STR(mdtest_show(md, "h"), "int[8=2*2*2] \"name\"\n");

	EXPECT_STR(mdtest_show(md, "x"), "a b[m] c");

	EXPECT_STR(mdtest_show(md, "f"),
		"a=a1[B],a2\n"
		"b[m]=b1[L],b2\n"
		"c=c1,c2\n"
	);

	EXPECT_STR(mdtest_show(md, "s"),
		"int          32       8       4\n"
		"a            32       2      16\n"
		"b            16       2       8\n"
		"c             8       2       4\n"
	);

	EXPECT_STR(mdtest_show(md, "d"),
		"a       b[m]    c           # int[8]\n"
		"a1[B]   b1[L]   c1          0 111\n"
		"a1[B]   b1[L]   c2          1 112\n"
		"a1[B]   b2      c1          2 121\n"
		"a1[B]   b2      c2          3 122\n"
		"a2      b1[L]   c1          4 211\n"
		"a2      b1[L]   c2          5 212\n"
		"a2      b2      c1          6 221\n"
		"a2      b2      c2          7 222\n"
	);

	EXPECT_STR(mdtest_show(md, "z"), "");

	md_time(md, "a");
	md_setflag(md, "c=c2", MDXFLAG_HIDE|MDXFLAG_TEMP,
		MDFLAG_HIDE|MDFLAG_TEMP);
	EXPECT_STR(mdtest_show(md, "f"),
		"a[t]=a1[B],a2\n"
		"b[m]=b1[L],b2\n"
		"c[h+]=c1,c2[H-]\n"
	);

	rd_deref(md);
}
