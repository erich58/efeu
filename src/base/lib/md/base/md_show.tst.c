#include <EFEU/UnitTest.h>
#include <EFEU/mdmat.h>
#include "utest/md.tst.h"

TEST_SUITE
{
	mdmat *md;
	SetupMdMat();

	EXPECT_PTR(md = md_scalar(int2Obj(42)));
	EXPECT_STR(mdtest_show(md, "d"),
		"42\n"
	);
	rd_deref(md);

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

	EXPECT_STR(mdtest_show(md, "T"), "int[8]");
	EXPECT_STR(mdtest_show(md, "t"), "int[8=2*2*2]");
	EXPECT_STR(mdtest_show(md, "h"), "int[8=2*2*2] \"name\"\n");

	EXPECT_STR(mdtest_show(md, "X"), "a b[m] c");

	EXPECT_STR(mdtest_show(md, "x"),
		"a=a1[B],a2\n"
		"b[m]=b1[L],b2\n"
		"c=c1,c2\n"
	);

	EXPECT_STR(mdtest_show(md, "S"),
		"int[8] a[2] b[2] c[2]\n"
	);

	EXPECT_STR(mdtest_show(md, "s"),
		" 32   8   4 int[8=2*2*2] \"name\"\n"
		" 32   2  16 a=a1[B],a2\n"
		" 16   2   8 b[m]=b1[L],b2\n"
		"  8   2   4 c=c1,c2\n"
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

	LMATCH_STR(mdtest_show(md, "z"),
		"@fussy\n"
		"T\tType with dimension\n"
		"t\tType with dimension composition\n"
		"h\tType and matrix name\n"
		"X\tAxis names\n"
		"x\tAxis structure with index list\n"
		"S\tType and axis dimension\n"
		"s\tAxis dimension and data size\n"
		"d\tData with labels\n"
	);

	md_time(md, "a");
	md_setflag(md, "c=c2", MDXFLAG_HIDE|MDXFLAG_TEMP,
		MDFLAG_HIDE|MDFLAG_TEMP);
	EXPECT_STR(mdtest_show(md, "x"),
		"a[t]=a1[B],a2\n"
		"b[m]=b1[L],b2\n"
		"c[h+]=c1,c2[H-]\n"
	);

	rd_deref(md);
}
