#include <EFEU/UnitTest.h>
#include <stdlib.h>

int hidden_main(int argc, char **argv);

#define	SRC2DOC "./src2doc"

#define	RUN_SRC2DOC(n, def)	\
TEST_FORK { \
	TEST_MKARG(main_args, UnitTest_tpl(SRC2DOC " " def)); \
	UnitTest_reopen(1, "stdout"); \
	UnitTest_reopen(2, "stderr"); \
	EXPECT(hidden_main(main_args->argc, main_args->argv) == 0); \
	TEST_EXIT; \
} CHILD_EXIT(0)


TEST_DECL(setup)
{
	setenv("LANG", "C", 1);
	PRINT_LABEL("Force separating UnitTest output from cmd output");
	UnitTest_stderr("stderr");
	UnitTest_stderr(NULL);
}

TEST_DECL(synopsis)
{
	TEST_FORK
	{
		setenv("TERM", "dumb", 1);
		setenv("COLUMNS", "60", 1);
		UnitTest_reopen(2, "stderr");
		char *args[] = { SRC2DOC, NULL };
		EXPECT(hidden_main(1, args) == 1);
		TEST_EXIT;
	}

	CHILD_EXIT(1);
	LMATCH_CONTENT("stderr",
		"SYNOPSIS\n"
		"src2doc\n"
	);
}

TEST_SUITE
{
	TEST_RUN(setup);
	TEST_RUN(synopsis);
}
