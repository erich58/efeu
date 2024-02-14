#include <EFEU/UnitTest.h>
#include <stdlib.h>

#define	MATCH(data, pat) \
do	{ const char *pattern = pat; \
	PRINT_STR(pattern); \
	MATCH_LINES(data, pattern); \
} while (0)

TEST_DECL(special)
{
	MATCH(NULL, NULL);
	MATCH("abc\nxyz\n", NULL);
	MATCH("abc\nxyz\n", "@sloppy");

	TEST_FORK
	{
		MATCH("abc\nxyz\n", "@fussy");
		MATCH(NULL, "abc\nxyz\n");
		MATCH("abc", "abc\n[x\nxyz\n");
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
	PRINT_STR(str);

	MATCH(str,
		"@fussy\n"
		"*extra\n"
		"*(x\n"
		"first\n"
		"second\n"
		"last\n"
		);

	TEST_FORK
	{
		MATCH(str, "second\n");
		MATCH(str, "first\nsecond\nlast\nextra\n");
		MATCH(str, "first\nlast\n");
		MATCH(str, "first\nother\nsecond\n");
		MATCH(str, "first\nother\nlast\n");
		TEST_EXIT;
	}

	CHILD_EXIT(1);
}

TEST_DECL(def_pat)
{
	const char *str =
		"some extra data\n"
		"The first line\n"
		"any other line\n"
		"The last line\n"
		"some extra data\n"
	;
	PRINT_STR(str);

	MATCH(str,
		"first\n"
		"last\n"
	);

	MATCH(str,
		"*extra\n"
		"first\n"
		"last\n"
	);

	TEST_FORK
	{
		MATCH(str,
			"@fussy\n"
			"*extra\n"
			"first\n"
			"last\n"
		);

		MATCH(str,
			"first\n"
			"missing\n"
		);

		TEST_EXIT;
	}

	CHILD_EXIT(1);
}

TEST_DECL(bad)
{
	TEST_FORK
	{
		MATCH(
			"first\n"
			"next\n"
			"extra\n"
			"something\n"
			"last\n"
		,
			"@fussy\n"
			"*extra\n"
			"first\n"
			"x[\n"
			"next\n"
			"abc\n"
			"xyz\n"
			"last\n"
		);

		TEST_EXIT;
	}

	CHILD_EXIT(1);
}

TEST_DECL(content)
{
	TEST_FILE("data",
		"The first line.\n"
		"The second line.\n"
		"The last line.\n"
	);
	LMATCH_CONTENT("data",
		"second\n"
	);
}

TEST_SUITE
{
	TEST_RUN(special);
	TEST_RUN(lmatch);
	TEST_RUN(def_pat);
	TEST_RUN(bad);
	TEST_RUN(content);
}
