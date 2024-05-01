#include <EFEU/UnitTest.h>
#include <EFEU/mdmat.h>
#include "utest/out.tst.h"

static const char *showflag(mdmat *md, int verbose)
{
	md_show(OUT(), md, verbose ? "x" : "X");
	return utest_out();
}

TEST_DECL(null)
{
	EXPECT_STR(showflag(NULL, 0), "");
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
	EXPECT_STR(showflag(md, 0), "a b c"); 

	EXPECT_PTR(md_time(md, "a"));
	EXPECT_STR(showflag(md, 0), "a[t] b c"); 

	EXPECT_PTR(md_hide(md, "b"));
	EXPECT_STR(showflag(md, 0), "a[t] b[h] c"); 

	EXPECT_PTR(md_mark(md, "c"));
	EXPECT_STR(showflag(md, 0), "a[t] b[h] c[m]"); 

	EXPECT_PTR(md_temp(md, "[bc]"));
	EXPECT_STR(showflag(md, 0), "a[t] b[h+] c[m+]"); 

	EXPECT_PTR(md_temp(md, NULL));
	EXPECT_STR(showflag(md, 0), "a[t] b[h] c[m]"); 

	EXPECT_PTR(md_mark(md, "a"));
	EXPECT_STR(showflag(md, 0), "a[mt] b[h] c"); 

	EXPECT_PTR(md_transp(md));
	EXPECT_STR(showflag(md, 0), "a[t] b[mh] c[m]"); 

	EXPECT_PTR(md_time(md, NULL));
	EXPECT_PTR(md_hide(md, NULL));
	EXPECT_PTR(md_mark(md, NULL));
	EXPECT_STR(showflag(md, 0), "a b c"); 

	EXPECT_PTR(md_setlock(md, "b=b2 c", MDXFLAG_TEMP, MDFLAG_TEMP));
	EXPECT_STR(showflag(md, 1),
		"a=a1,a2,a3\n"
		"b[+]=b1[-],b2,b3[-]\n"
		"c[+]=c1,c2,c3\n"
	); 
	EXPECT_PTR(md_clrflag(md, MDXFLAG_TEMP, MDFLAG_TEMP));

	EXPECT_STR(showflag(md, 1),
		"a=a1,a2,a3\n"
		"b=b1,b2,b3\n"
		"c=c1,c2,c3\n"
	); 

	EXPECT_PTR(md_base(md, "b=b2 c"));
	EXPECT_STR(showflag(md, 1),
		"a=a1,a2,a3\n"
		"b=b1,b2[B],b3\n"
		"c=c1[B],c2[B],c3[B]\n"
	); 
	EXPECT_PTR(md_base(md, "a"));
	EXPECT_STR(showflag(md, 1),
		"a=a1[B],a2[B],a3[B]\n"
		"b=b1,b2,b3\n"
		"c=c1,c2,c3\n"
	); 
	EXPECT_PTR(md_base(md, NULL));
	EXPECT_STR(showflag(md, 1),
		"a=a1,a2,a3\n"
		"b=b1,b2,b3\n"
		"c=c1,c2,c3\n"
	); 

	EXPECT_PTR(md_setlock(md, "b=b2 c", MDXFLAG_MARK, MDFLAG_TEMP));
	EXPECT_STR(showflag(md, 1),
		"a=a1,a2,a3\n"
		"b[m]=b1[-],b2,b3[-]\n"
		"c[m]=c1,c2,c3\n"
	); 
	EXPECT_PTR(md_clrflag(md, MDXFLAG_MARK, MDFLAG_TEMP));
	EXPECT_STR(showflag(md, 1),
		"a=a1,a2,a3\n"
		"b=b1,b2,b3\n"
		"c=c1,c2,c3\n"
	); 

	EXPECT_PTR(md_select(md, "b=b2 c"));
	EXPECT_STR(showflag(md, 1),
		"a=a1,a2,a3\n"
		"b=b1[L],b2,b3[L]\n"
		"c=c1,c2,c3\n"
	); 
	EXPECT_PTR(md_select(md, NULL));
	EXPECT_STR(showflag(md, 1),
		"a=a1,a2,a3\n"
		"b=b1,b2,b3\n"
		"c=c1,c2,c3\n"
	); 

	EXPECT_PTR(md_setflag(md, "b=b2 c", MDXFLAG_HIDE, MDFLAG_HIDE));
	EXPECT_STR(showflag(md, 1),
		"a=a1,a2,a3\n"
		"b[h]=b1,b2[H],b3\n"
		"c[h]=c1[H],c2[H],c3[H]\n"
	); 
	EXPECT_PTR(md_setflag(md, NULL, MDXFLAG_HIDE, MDFLAG_HIDE));
	EXPECT_STR(showflag(md, 1),
		"a=a1,a2,a3\n"
		"b=b1,b2,b3\n"
		"c=c1,c2,c3\n"
	); 

	CLEANUP_OUT;
}
