#include <EFEU/UnitTest.h>
#include <unistd.h>

TEST_SUITE
{
	TEST_FORK
	{
		PRINT_LABEL("Child");
		EXPECT_TRUE(0);
		TEST_EXIT;
	}

	CHILD_EXIT(1);
}
