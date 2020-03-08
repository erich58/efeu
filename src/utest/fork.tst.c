#include <EFEU/UnitTest.h>
#include <unistd.h>
#include <stdlib.h>

TEST_SUITE
{
	PRINT_LABEL("Parent");

	TEST_FORK
	{
		PRINT_LABEL("Child");

		TEST_FORK
		{
			PRINT_LABEL("Grandchild");
			abort();
			TEST_EXIT; // not reached
		}

		CHILD_SIGNALED(6);
		TEST_EXIT;
	}

	CHILD_EXIT(0);

	TEST_FORK
	{
		PRINT_LABEL("Exec");
		execl("/bin/false", "false", NULL);
		TEST_EXIT; // not reached
	}

	CHILD_EXIT(1);
}
