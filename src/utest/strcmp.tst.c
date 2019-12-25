#include <EFEU/UnitTest.h>

TEST_SUITE
{
	EXPECT_STRCMP(NULL, ==, NULL);
	EXPECT_STRCMP(NULL, <, "");
	EXPECT_STRCMP("", >, NULL);
	EXPECT_STRCMP("", ==, "");
	EXPECT_STRCMP("abc", <, "abcd");
	EXPECT_STRCMP("1", >, "0");

	EXPECT_STREQ("a", "a");
	EXPECT_STRNE("a", "b");
}
