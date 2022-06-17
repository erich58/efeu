#include <EFEU/UnitTest.h>
#include <unistd.h>
#include <stdlib.h>

TEST_SUITE
{
	PRINT_LABEL("Parent");
	EXPECT(UnitTest_wait() == 0);

	TEST_FORK
	{
		PRINT_LABEL("Child");
		EXPECT(UnitTest_child == 0);
		UnitTest_stdout("stdout");

		TEST_FORK
		{
			PRINT_LABEL("Grandchild");
			ASSERT(1);
			ASSERT(0);
			TEST_EXIT; // not reached
		}

		CHILD_EXIT(2);
		MATCH_CONTENT("stdout", "ABORT\n");
		TEST_EXIT;
	}

	EXPECT(UnitTest_child != 0);
	CHILD_EXIT(0);

	TEST_FORK
	{
		PRINT_LABEL("Abort with signal");
		abort();
		TEST_EXIT; // not reached
	}

	CHILD_SIGNALED(6);
}
