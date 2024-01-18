#include <EFEU/UnitTest.h>
#include <EFEU/mdmat.h>

TEST_DECL(null)
{
	EXPECT_STR(md_showflag(NULL, 0), NULL);
	EXPECT(md_mark(NULL, NULL) == NULL);
	EXPECT(md_time(NULL, NULL) == NULL);
	EXPECT(md_hide(NULL, NULL) == NULL);
}

TEST_SUITE
{
	mdmat *md;

	TEST_RUN(null);

	EXPECT_PTR(md = md_create(&Type_int,
		"a=a1,a2,a3 b=b1,b2,b3 c=c1,c2,c3"));
	EXPECT_STR(md_showflag(md, 0), "a b c"); 

	EXPECT_PTR(md_time(md, "a"));
	EXPECT_STR(md_showflag(md, 0), "a[t] b c"); 

	EXPECT_PTR(md_hide(md, "b"));
	EXPECT_STR(md_showflag(md, 0), "a[t] b[h] c"); 

	EXPECT_PTR(md_mark(md, "c"));
	EXPECT_STR(md_showflag(md, 0), "a[t] b[h] c[m]"); 

	EXPECT_PTR(md_temp(md, "[bc]"));
	EXPECT_STR(md_showflag(md, 0), "a[t] b[h+] c[m+]"); 

	EXPECT_PTR(md_temp(md, NULL));
	EXPECT_STR(md_showflag(md, 0), "a[t] b[h] c[m]"); 

	EXPECT_PTR(md_mark(md, "a"));
	EXPECT_STR(md_showflag(md, 0), "a[mt] b[h] c"); 

	EXPECT_PTR(md_mark(md, "~*"));
	EXPECT_STR(md_showflag(md, 0), "a[t] b[mh] c[m]"); 

	EXPECT_PTR(md_mark(md, "!b"));
	EXPECT_STR(md_showflag(md, 0), "a[t] b[h] c[m]"); 

	EXPECT_PTR(md_time(md, NULL));
	EXPECT_PTR(md_hide(md, NULL));
	EXPECT_PTR(md_mark(md, NULL));
	EXPECT_STR(md_showflag(md, 0), "a b c"); 

	md_setflag(md, "b=b2 c", 0, mdsf_mark, MDXFLAG_TEMP, mdsf_lock, MDFLAG_TEMP);
	EXPECT_STR(md_showflag(md, 1),
		"a=a1,a2,a3\n"
		"b[+]=b1[-],b2,b3[-]\n"
		"c[+]=c1,c2,c3\n"
	); 
	md_allflag(md, 0, mdsf_clear, MDXFLAG_TEMP, mdsf_clear, MDFLAG_TEMP);
}
