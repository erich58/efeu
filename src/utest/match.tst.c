#include <EFEU/UnitTest.h>

TEST_SUITE
{
	MATCH_STR(NULL, NULL);
	EXPECT_STRCMP("x", !~, NULL);
	EXPECT_STRCMP(NULL, !~, ".");
	MATCH_STR("", "");
	EXPECT_STRCMP("", !~, ".");
	MATCH_STR("", ".*");
	MATCH_STR("abc", "b");
	EXPECT_STRCMP("abc", !~, "^b");
	MATCH_STR("abc", "!^b");
	MATCH_STR("a12345b", "a[0-9]*b");
	EXPECT_STRCMP("abc", !~, "[x"); /* Syntax Error */
	MATCH_STR("abc\ndef", "^abc\ndef$");
	EXPECT_STRCMP("abc\ndef", !~, "^abc\nde$");
	MATCH_STR("2000-12-01", "[0-9]{4}-[0-9]{2}-[0-9]{2}");
	EXPECT_STRCMP("20-12-01", !~, "[0-9]{4}-[0-9]{2}-[0-9]{2}");

	TEST_FORK
	{
		char *x = "abc";
		MATCH_STR(x, "^b");
		TEST_EXIT;
	}

	CHILD_EXIT(1);
}
