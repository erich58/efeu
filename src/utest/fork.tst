#include <EFEU/UnitTest.h>

TEST_SUITE
{
	TEST_FORK
	{
		PRINT_STR("Child");
		TEST_EXIT;
	}

	PRINT_STR("Parent");
	TEST_WAIT;
}
