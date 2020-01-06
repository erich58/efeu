#include <EFEU/UnitTest.h>

TEST_DECL(test1)
{
	PRINT_LABEL("Test 1");
}

TEST_DECL(test2)
{
	PRINT_LABEL("Test 2");
}

TEST_SUITE
{
	PRINT_LABEL("main test");
	TEST_RUN(test1);
	TEST_RUN(test2);
}
