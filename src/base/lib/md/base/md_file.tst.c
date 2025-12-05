#include <EFEU/UnitTest.h>
#include <EFEU/mdmat.h>
#include <EFEU/utest/md.tst.h>
#include <EFEU/utest/out.tst.h>

TEST_SUITE
{
	setenv("LANG", "C", 1);
	SetupMdMat();
	SetProgName("ID");

	mdmat *md = mdtest_create("Title r=a,b c=x,y"); 

	UnitTest_stderr("stderr");
	TEST_EXPR(md_file(OUT(), "/dev/null", NULL));
	EXPECT_OUT(UnitTest_tpl(""));
	UnitTest_stderr(NULL);
	LMATCH_CONTENT("stderr", "WNG ID: mmap\\(\"/dev/null\"):\n");

	TEST_EXPR(md_fsave(UnitTest_tmp("a"), md, 0));
	TEST_EXPR(md_file(OUT(), UnitTest_tpl("@/a"), NULL));
	EXPECT_OUT(UnitTest_tpl("@/a: int[4=2*2] \"Title\"\n"));
	TEST_EXPR(md_file(OUT(), UnitTest_tpl("|cat @/a"), NULL));
	EXPECT_OUT(UnitTest_tpl("|cat @/a: int[4=2*2] \"Title\"\n"));

	md_fsave(UnitTest_tmp("a.gz"), md, 0);
	TEST_EXPR(md_file(OUT(), UnitTest_tpl("@/a.gz"), NULL));
	EXPECT_OUT(UnitTest_tpl("@/a.gz: int[4=2*2] \"Title\"\n"));

	md_fputmap(md, UnitTest_tmp("a.map"));
	TEST_EXPR(md_file(OUT(), UnitTest_tpl("@/a.map"), NULL));
	EXPECT_OUT(UnitTest_tpl("@/a.map: int[4=2*2] \"Title\"\n"));

	TEST_EXPR(md_info(OUT(), md, "Name", NULL));
	EXPECT_OUT("Name: int[4=2*2] \"Title\"\n");

	TEST_EXPR(md_info(OUT(), md, "Name", "s"));
	EXPECT_OUT("Name: int[4] r[2] c[2]\n");

	TEST_EXPR(md_info(OUT(), md, "Name", "x"));
	EXPECT_OUT("Name: int[4=2*2] \"Title\"\n"
		"r=a,b\n"
		"c=x,y\n"
	);

	CLEANUP_OUT;
}
