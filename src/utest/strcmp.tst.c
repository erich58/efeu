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

	EXPECT_STR("a", "a");

	TEST_FORK
	{
		char *x = "x";
		char *y = "y";
		EXPECT_STR(x, "a");
		EXPECT_STR("a", y);
		EXPECT_STR(x, y);
		TEST_EXIT;
	}

	CHILD_EXIT(1);

	/* Sonderfall */
	UnitTest_strcheck(__FILE__, __LINE__, "==", "abc", "abc", "expr", NULL);
}
