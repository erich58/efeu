#include <EFEU/UnitTest.h>
#include <stdlib.h>

#define	ESH	BUILD "/esh"

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
		CMD_EXEC(ESH, "esh -?");
	}

	CHILD_EXIT(0);
	LMATCH_CONTENT("stderr", "SYNOPSIS\n");
}

TEST_SUITE
{
	TEST_RUN(setup);
	TEST_RUN(synopsis);
}
