#include <EFEU/UnitTest.h>
#include <EFEU/mdmat.h>

TEST_SUITE
{
	mdmat *md;

	SetupMdMat();

	EXPECT_INT(md_paste(NULL, "x", ".", 0, 1), 0);

	EXPECT_PTR(md = md_create(&Type_int, "a=a1,a2 b=b1,b2 c=c1,c2"));
	EXPECT_INT(md_paste(md, "x", ".", -2, 0), 2);
	EXPECT_STR(md_showflag(md, 1),
		"a=a1,a2\n"
		"x=b1.c1,b1.c2,b2.c1,b2.c2\n"
	); 
	rd_deref(md);

	EXPECT_PTR(md = md_create(&Type_int, "a=a1,a2 b=b1,b2 c=c1,c2"));
	EXPECT_INT(md_paste(md, NULL, ".", 0, 1), 1);
	EXPECT_STR(md_showflag(md, 1),
		"0=a1,a2\n"
		"b=b1,b2\n"
		"c=c1,c2\n"
	); 
	EXPECT_INT(md_paste(md, "x", ".", 3, 1), 1);
	EXPECT_STR(md_showflag(md, 1),
		"0=a1,a2\n"
		"b=b1,b2\n"
		"x=c1,c2\n"
	); 
	rd_deref(md);
}
