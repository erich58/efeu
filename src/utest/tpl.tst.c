#include <EFEU/UnitTest.h>
#include <stdlib.h>
#include <dirent.h>

TEST_DECL(tpl)
{
	EXPECT_STR(UnitTest_tpl(NULL), NULL);
	EXPECT_STR(UnitTest_tpl("@"), UnitTest_tmp(NULL));
	EXPECT_STR(UnitTest_tpl("@/x\\@bc"), UnitTest_tmp("x@bc"));
	EXPECT_STR(UnitTest_tpl("abc"), "abc");
	EXPECT_STR(UnitTest_tpl("abc\\"), "abc");
	EXPECT_STR(UnitTest_tpl("abc\\x"), "abcx");
	EXPECT_STR(UnitTest_tpl("abc\\\\x"), "abc\\x");
	EXPECT_STR(UnitTest_tpl("abc-{"), "abc-");
	EXPECT_STR(UnitTest_tpl("abc-}"), "abc-}");
	EXPECT_STR(UnitTest_tpl("abc-{}-x"), "abc--x");
	EXPECT_STR(UnitTest_tpl("abc-{\\}-x"), "abc-\\-x");
	EXPECT_STR(UnitTest_tpl("abc-\\{}-x"), "abc-{}-x");
	EXPECT_STR(UnitTest_tpl("abc-\\{name}-x"), "abc-{name}-x");
	EXPECT_STR(UnitTest_tpl("abc-{name"), "abc-name");
	EXPECT_STR(UnitTest_tpl("abc-{name}-x"), "abc-name-x");
	PRINT_STR(UnitTest_tpl("{?}"));
	UnitTest_tplpar("name", "data");
	EXPECT_STR(UnitTest_tpl("abc-{name}-x"), "abc-data-x");
	UnitTest_tplpar("abc", "123");
	UnitTest_tplpar("name", "xxx");
	EXPECT_STR(UnitTest_tpl("abc-{name}-x"), "abc-xxx-x");
	PRINT_STR(UnitTest_tpl("{?}"));
	UnitTest_tplpar("xy", NULL);
	UnitTest_tplpar("name", NULL);
	PRINT_STR(UnitTest_tpl("{?}"));
	UnitTest_tplpar(NULL, NULL);
	PRINT_STR(UnitTest_tpl("{?}"));
}

TEST_SUITE
{
	TEST_RUN(tpl);
}
