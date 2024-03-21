#include <EFEU/UnitTest.h>
#include <EFEU/mstring.h>
#include <stdlib.h>

#define	RT_BIN	BUILD "/efeuman"	// Binary version with coverage
#define	RT_NAME	TOP "/bin/efeuman"	// Runtime name (defines ApplPath)

#define	RUN_EFEUMAN(args)	\
TEST_FORK { \
	UnitTest_reopen(1, "stdout"); \
	UnitTest_reopen(2, "stderr"); \
	CMD_EXEC(RT_BIN, RT_NAME " " args); \
} CHILD_EXIT(0)

TEST_DECL(setup)
{
	setenv("LANG", "C", 1);
	UnitTest_newpath(TOP "/bin");
	UnitTest_mkcmd("efeudoc", "#!/bin/sh\ncat\n");
	PRINT_LABEL("Force separating UnitTest output from cmd output");
	UnitTest_stderr("stderr");
	UnitTest_stderr(NULL);
}

TEST_DECL(hello)
{
	TEST_FILE("hello.sh",
		"#!/bin/sh\n"
		"# Hello World\n"
		"# $Copyright (C) Name\n"
		"\n"
		"# $pconfig\n"
		"# Version = 42\n"
		"# *msg |\n"
		"#	message (instead of Hello World)\n"
		"\n"
		"# $Description\n"
		"# Hello World Example\n"
		"echo Hello World\n"
	);

	RUN_EFEUMAN("-- @/hello.sh -?");
	EXPECT_CONTENT("stderr",
		"\\Synopsis\n"
		"\\hang\n"
		"|hello.sh|\n"
		"\\[~|--help|[|=|</type/>]~]\n"
		"\\[~|--version|~]\n"
		"\\[~</msg/>~]\n"
		"\\end\n"
	);

	RUN_EFEUMAN("-- @/hello.sh --version");
	EXPECT_CONTENT("stderr",
		"hello.sh -- Hello World\n"
		"\n"
		"\n"
		"Version: 42\n"
		"\n"
		"(C) Name\n"
	);

	RUN_EFEUMAN("-- @/hello.sh --help=raw\n");
	PRINT_CONTENT("stdout");
}

TEST_DECL(plain)
{
	TEST_FILE("plain.sh",
		"#!/bin/sh\n"
		"echo Test\n"
	);

	RUN_EFEUMAN("-- @/plain.sh -?");
	EXPECT_CONTENT("stderr",
		"\\Synopsis\n"
		"\\hang\n"
		"|plain.sh|\n"
		"\\[~|--help|[|=|</type/>]~]\n"
		"\\[~|--version|~]\n"
		"\\end\n"
	);

	RUN_EFEUMAN("-v Version=42 -- @/plain.sh --version");
	EXPECT_CONTENT("stderr",
		"plain.sh -- plain.sh\n"
		"\n"
		"Version: 42\n"
		"\n"
	);
}

TEST_SUITE
{
	TEST_RUN(setup);
	TEST_RUN(hello);
	TEST_RUN(plain);
}
