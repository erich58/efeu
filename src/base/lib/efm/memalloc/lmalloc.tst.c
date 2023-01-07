#include <EFEU/UnitTest.h>
#include <EFEU/Debug.h>
#include <stdio.h>
#include <signal.h>

TEST_DECL(errcheck)
{
	char *p = NULL;

	EXPECT(realloc_errcheck(NULL, 0) == NULL);
	EXPECT((p = realloc_errcheck(NULL, 32)) != NULL);
	EXPECT((p = realloc_errcheck(p, 0)) == NULL);

	UnitTest_stderr("stderr");

	TEST_FORK
	{
		realloc_errcheck(NULL, (size_t) ~0 >> 1); exit(0);
	}

	CHILD_EXIT(1);
	UnitTest_stderr(NULL);
	MATCH_CONTENT("stderr", "sorry: realloc.*failed");
}

static void *fake_ptr = (void *) 0xFF;
static int fake_count = 0;

static void *realloc_fake(void *data, size_t size)
{
	fake_count++;
	return size ? fake_ptr : NULL;
	return NULL;
}

TEST_DECL(lmcfg)
{
	char *p = NULL;
	EXPECT((p = lrealloc(p, 32)) != NULL);

	/* Use fake reallocator */

	lmcfg(realloc_fake);
	EXPECT(fake_count == 0);
	EXPECT(lrealloc(NULL, 0) == NULL);
	EXPECT(fake_count == 0);
	EXPECT(lrealloc(p, 32) == fake_ptr);
	EXPECT(fake_count == 1);
	EXPECT(lmalloc(32) == fake_ptr);
	EXPECT(fake_count == 2);
	lfree(p);
	EXPECT(fake_count == 3);
	lmcfg(NULL);

	/* Restore default behavior */

	EXPECT(lrealloc(p, 32) == p);
	EXPECT(fake_count == 3);
}

TEST_DECL(lmalloc)
{
	char *p = NULL;

	EXPECT((p = lrealloc(p, 32)) != NULL);
	EXPECT((p = lrealloc(p, 64)) != NULL);
	EXPECT((p = lrealloc(p, 0)) == NULL);
}

TEST_SUITE
{
	TEST_RUN(errcheck);
	TEST_RUN(lmcfg);
	TEST_RUN(lmalloc);
}
