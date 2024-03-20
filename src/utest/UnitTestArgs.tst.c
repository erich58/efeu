#include <EFEU/UnitTest.h>
#include <stdlib.h>

TEST_SUITE
{
	TEST_MKARG(args, " a \t@/b \\@ 'a \tb' \\");
	EXPECT_INT(args->argc, 5);
	EXPECT_STR(args->argv[0], "a");
	EXPECT_STR(args->argv[1], UnitTest_tmp("b"));
	EXPECT_STR(args->argv[2], "@");
	EXPECT_STR(args->argv[3], "a \tb");
	EXPECT_STR(args->argv[4], "");
	EXPECT_STR(args->argv[5], NULL);

	TEST_MKARG(empty, NULL);
	EXPECT_INT(empty->argc, 0);
	EXPECT_STR(empty->argv[0], NULL);

	UnitTestArgs_clear(empty);
	EXPECT_INT(empty->argc, 0);
	EXPECT(empty->argv == NULL);
}
