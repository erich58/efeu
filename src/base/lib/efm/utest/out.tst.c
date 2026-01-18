#include <EFEU/utest/out.tst.h>

TEST_SUITE
{
	EXPECT_OUT("");
	TEST_EXPR(io_puts("lorem", OUT()));
	EXPECT_OUT("lorem");
	EXPECT_OUT("");

	TEST_EXPR(io_puts("ipsum", OUT()));
	EXPECT_NOUT(expression, "ipsum");

	TEST_EXPR(io_puts("dolor", OUT()));
	MATCH_OUT("dol");

	TEST_EXPR(io_puts("sit", OUT()));
	MATCH_NOUT(expression, "s");

	TEST_EXPR(io_puts("amet\n", OUT()));
	LMATCH_OUT("am\n");

	TEST_EXPR(io_puts("consectetur\n", OUT()));
	LMATCH_NOUT(expression, "cons\n");

	TEST_EXPR(io_puts("adipiscing\n", OUT()));
	PRINT_OUT();

	TEST_EXPR(io_puts("elit\n", OUT()));
	PRINT_NOUT(expression);

	CLEANUP_OUT;
}
