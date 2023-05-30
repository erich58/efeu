#include <EFEU/UnitTest.h>
#include <stdlib.h>

TEST_SUITE
{
	TEST_ARGV(argv, 4, "a b c d");
	EXPECT_STR(argv[0], "a");
	EXPECT_STR(argv[1], "b");
	EXPECT_STR(argv[2], "c");
	EXPECT_STR(argv[3], "d");
	EXPECT_STR(argv[4], NULL);
	free(argv);

	TEST_ARGV(a1, 0, "a b c d");
	EXPECT_STR(a1[0], NULL);
	free(a1);

	TEST_ARGV(a2, 1, NULL);
	EXPECT_STR(a2[0], "");
	EXPECT_STR(a2[1], NULL);
	free(a2);

	TEST_ARGV(b, 6, "a b  c d");
	EXPECT_STR(b[0], "a");
	EXPECT_STR(b[1], "b");
	EXPECT_STR(b[2], "");
	EXPECT_STR(b[3], "c");
	EXPECT_STR(b[4], "d");
	EXPECT_STR(b[5], "");
	EXPECT_STR(b[6], NULL);
	free(b);

	TEST_ARGV2(c, 5, "a|b c||d", '|');
	EXPECT_STR(c[0], "a");
	EXPECT_STR(c[1], "b c");
	EXPECT_STR(c[2], "");
	EXPECT_STR(c[3], "d");
	EXPECT_STR(c[4], "");
	EXPECT_STR(c[5], NULL);
	free(c);
}
