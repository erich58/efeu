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
		MATCH_CONTENT("stdout", "ABORT");
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

	TEST_FORK
	{
		TEST_FORK { exit(0); }
		CHILD_SIGNALED(6);

		TEST_FORK { abort(); }
		CHILD_EXIT(0);

		TEST_EXIT;
	}

	CHILD_EXIT(1);
	EXPECT_INT(UnitTest_system("true"), 0);
	EXPECT_INT(UnitTest_system("false"), 1);
	EXPECT_INT(UnitTest_system("exit 42"), 42);
	EXPECT_INT(UnitTest_system("kill -15 $$"), -15);
}
