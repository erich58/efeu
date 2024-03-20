#include <EFEU/UnitTest.h>
#include <unistd.h>
#include <stdlib.h>

TEST_SUITE
{
	if	(UnitTest_argc == 1)
	{
		PRINT_LABEL("Restart with argument");
		execlp(UnitTest_argv[0], UnitTest_name, "arg", NULL);
	}

	EXPECT_STR(UnitTest_argv[0], UnitTest_name);
	EXPECT_STR(UnitTest_argv[1], "arg");
}
