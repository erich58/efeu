#include <EFEU/UnitTest.h>

TEST_SUITE
{
	EXPECT_STRCMP(NULL, ==, NULL);
	EXPECT_STRCMP(NULL, <, "");
	EXPECT_STRCMP("", >, NULL);
	EXPECT_STRCMP("", ==, "");
	EXPECT_STRCMP("abc", <, "abcd");
	EXPECT_STRCMP("abc", <=, "abc");
	EXPECT_STRCMP("1", >, "0");
	EXPECT_STRCMP("1", >=, "1");

	EXPECT_STREQ("a", "a");
	EXPECT_STRNE("a", "b");

	TEST_FORK
	{
		char *x = "x";
		char *y = "y";
		EXPECT_STREQ(x, "a");
		EXPECT_STREQ("a", y);
		EXPECT_STREQ(x, y);
		TEST_EXIT;
	}

	CHILD_EXIT(1);
}
