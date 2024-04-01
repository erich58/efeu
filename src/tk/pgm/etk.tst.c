#include <EFEU/UnitTest.h>
#include <stdlib.h>

#define	ETK_BIN		BUILD "/etk"
#define	ETK_NAME	TOP "/bin/etk"

TEST_SUITE
{
	setenv("LANG", "C", 1);
	UnitTest_stderr("stderr");

	TEST_FORK
	{
		UnitTest_mkcmd("efeudoc", "#!/bin/sh\ncat\n");
		CMD_EXEC(ETK_BIN, ETK_NAME " -?");
	}

	UnitTest_stderr(NULL);
	CHILD_EXIT(0);
	EXPECT_CONTENT("stderr",
		"\\Synopsis\n"
		"\\hang\n"
		"|etk|\n"
		"\\[~|--help|[|=|</type/>]~]\n"
		"\\[~|--version|~]\n"
		"\\[~|--info|[|=|</entry/>]~]\n"
		"\\[~|--debug|[|=|</mode/>]~]\n"
		"\\[~|--verbose|~]\n"
		"\\[~|-I|~</dir/>~]\n"
		"\\[~|-D|~</name|=|val/>~]\n"
		"\\[~|-c|~</string/>~]\n"
		"\\[~</file/>~]\n"
		"\\[~</arg(s)/>~]\n"
		"\\end\n"
	);
}
