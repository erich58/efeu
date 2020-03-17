#include <EFEU/UnitTest.h>

TEST_SUITE
{
	EXPECT_MATCH(NULL, NULL);
	EXPECT_NOMATCH("x", NULL);
	EXPECT_NOMATCH(NULL, ".");
	EXPECT_MATCH("", "");
	EXPECT_NOMATCH("", ".");
	EXPECT_MATCH("", ".*");
	EXPECT_MATCH("abc", "b");
	EXPECT_NOMATCH("abc", "^b");
	EXPECT_MATCH("abc", "!^b");
	EXPECT_MATCH("a12345b", "a[0-9]*b");
	EXPECT_NOMATCH("abc", "[x"); /* Syntax Error */
	EXPECT_MATCH("abc\ndef", "^abc\ndef$");
	EXPECT_NOMATCH("abc\ndef", "^abc\nde$");
	EXPECT_MATCH("2000-12-01", "[0-9]{4}-[0-9]{2}-[0-9]{2}");
	EXPECT_NOMATCH("20-12-01", "[0-9]{4}-[0-9]{2}-[0-9]{2}");

	TEST_FORK
	{
		char *x = "abc";
		EXPECT_MATCH(x, "^b");
		TEST_EXIT;
	}

	CHILD_EXIT(1);
}
