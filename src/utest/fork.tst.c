#include <EFEU/UnitTest.h>
#include <unistd.h>

TEST_SUITE
{
	PRINT_LABEL("Parent");

	TEST_FORK
	{
		PRINT_LABEL("Child");

		TEST_FORK
		{
			PRINT_LABEL("Grandchild");

			TEST_EXIT;
		}

		TEST_WAIT;
		TEST_EXIT;
	}

//	EXPECT_TRUE(usleep(1000) == 0);
	TEST_WAIT;
}
