#include <EFEU/UnitTest.h>
#include <EFEU/memalloc.h>

TEST_DECL(realloc)
{
	char *p, *p2;

	EXPECT(realloc_errcheck(NULL, 0) == NULL);
	EXPECT(realloc_limcheck(NULL, 0) == NULL);

	EXPECT((p = realloc_limcheck(NULL, 32)) != NULL);
	EXPECT((p = realloc_limcheck(p, 64)) != NULL);
	EXPECT((p2 = realloc_limcheck(NULL, 32)) != NULL);
	EXPECT((p = realloc_limcheck(p, 0)) == NULL);

	UnitTest_stderr("stderr");

	TEST_FORK
	{
		realloc_limcheck(NULL, (size_t) ~0 >> 1); exit(0);
	}

	CHILD_EXIT(1);
	UnitTest_stderr(NULL);
	MATCH_CONTENT("stderr", "sorry: realloc.*failed");
}

TEST_DECL(overwrite)
{
	UnitTest_stderr("stderr");

	TEST_FORK
	{
		char *p = realloc_limcheck(NULL, 8);
		memset(p, 'x', 10);
		realloc_limcheck(NULL, 8); exit(0);
	}

	CHILD_EXIT(1);
	UnitTest_stderr(NULL);
	MATCH_CONTENT("stderr", "limcheck.*memory overwrite detected");
}

TEST_SUITE
{
	TEST_RUN(realloc);
	TEST_RUN(overwrite);
}
