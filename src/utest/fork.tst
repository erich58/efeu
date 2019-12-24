#include <EFEU/UnitTest.h>

UNIT_TEST
{
	TEST_FORK
	{
		PRINT_STR("Child");
		TEST_EXIT;
	}

	PRINT_STR("Parent");
	TEST_WAIT;
}
