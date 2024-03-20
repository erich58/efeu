#include <EFEU/UnitTest.h>
#include <stdlib.h>

TEST_SUITE
{
	UnitTestArgs *args = UnitTestArgs_init(NULL, " a \t@/b \\@ 'a \tb' \\");
	EXPECT_INT(args->argc, 5);
	EXPECT_STR(args->argv[0], "a");
	EXPECT_STR(args->argv[1], UnitTest_tmp("b"));
	EXPECT_STR(args->argv[2], "@");
	EXPECT_STR(args->argv[3], "a \tb");
	EXPECT_STR(args->argv[4], "");
	EXPECT_STR(args->argv[5], NULL);
	UnitTestArgs_init(NULL, NULL);
	EXPECT_INT(args->argc, 0);
	EXPECT_STR(args->argv, NULL);
}
