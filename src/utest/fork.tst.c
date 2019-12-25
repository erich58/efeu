#include <EFEU/UnitTest.h>
#include <unistd.h>

TEST_SUITE
{
	TEST_FORK
	{
		PRINT_LABEL("Child");
		TEST_EXIT;
	}

	PRINT_LABEL("Parent");
	EXPECT_TRUE(usleep(1000) == 0);
	TEST_WAIT;
	EXPECT_TRUE(UnitTest_wait() == 0);
}
