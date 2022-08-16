#include <EFEU/UnitTest.h>
#include <stdlib.h>

TEST_DECL(special)
{
	MATCH_LINES(NULL, NULL);

	TEST_FORK
	{
		MATCH_LINES("abc\nxyz\n", NULL);
		MATCH_LINES(NULL, "abc\nxyz\n");
		MATCH_LINES("abc", "abc\n[x\nxyz\n");
		TEST_EXIT;
	}

	CHILD_EXIT(1);
}

TEST_DECL(lmatch)
{
	const char *str =
		"The first line.\n"
		"The second line.\n"
		"The last line.\n"
	;

	MATCH_LINES(str, "first\nsecond\nlast\n");

	TEST_FORK
	{
		MATCH_LINES(str, "second\n");
		MATCH_LINES(str, "first\nsecond\nlast\nextra\n");
		MATCH_LINES(str, "first\nlast\n");
		MATCH_LINES(str, "first\nother\nlast\n");
		TEST_EXIT;
	}

	CHILD_EXIT(1);
}

TEST_SUITE
{
	TEST_RUN(special);
	TEST_RUN(lmatch);
}
