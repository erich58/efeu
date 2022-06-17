#include <EFEU/UnitTest.h>

TEST_SUITE
{
	TEST_FORK
	{
		int a;
		TEST_EXPR(a = 42);
		EXPECT_INT(a, ~0);
		TEST_EXIT;
	}

	CHILD_EXIT(1);

	TEST_FORK
	{
		size_t z;
		TEST_EXPR(z = -1);
		EXPECT_UINT(z, (size_t) 0);
		TEST_EXIT;
	}

	CHILD_EXIT(1);

	TEST_FORK
	{
		int c;
		TEST_EXPR(c = '\n');
		EXPECT_CHAR(c, -1);
		TEST_EXIT;
	}

	CHILD_EXIT(1);
}
