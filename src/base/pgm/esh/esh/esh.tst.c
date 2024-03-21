#include <EFEU/UnitTest.h>
#include <stdlib.h>

#define	ESH_BIN		BUILD "/esh"
#define	ESH_NAME	TOP "/bin/esh"

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
		UnitTest_mkcmd("efeudoc", "#!/bin/sh\ncat\n");
		UnitTest_reopen(2, "stderr");
		CMD_EXEC(ESH_BIN, ESH_NAME " -?");
	}

	CHILD_EXIT(0);
	EXPECT_CONTENT("stderr",
		"\\Synopsis\n"
		"\\hang\n"
		"|esh|\n"
		"\\[~|--help|[|=|</type/>]~]\n"
		"\\[~|--version|~]\n"
		"\\[~|--info|[|=|</entry/>]~]\n"
		"\\[~|--debug|[|=|</mode/>]~]\n"
		"\\[~|--verbose|~]\n"
		"\\[~|-I|~</dir/>~]\n"
		"\\[~|-D|~</name|=|val/>~]\n"
		"\\[~|-c|~</string/>~]\n"
		"\\[~|-E|~]\n"
		"\\[~</file/>~]\n"
		"\\[~</arg(s)/>~]\n"
		"\\end\n"
	);
}

TEST_SUITE
{
	TEST_RUN(setup);
	TEST_RUN(synopsis);
}
