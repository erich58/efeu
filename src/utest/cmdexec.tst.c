#include <EFEU/UnitTest.h>
#include <unistd.h>
#include <stdlib.h>

TEST_SUITE
{
	UnitTest_tmp(NULL);

	TEST_FORK
	{
		UnitTest_reopen(2, "stderr");
		CMD_EXEC(NULL, NULL);
	}

	CHILD_EXIT(1);
	LMATCH_CONTENT("stderr", "cmdexec: incomplete command specification");

	TEST_FORK
	{
		UnitTest_reopen(2, "stderr");
		CMD_EXEC("/bad_command", "name");
	}

	CHILD_EXIT(1);
	LMATCH_CONTENT("stderr", "/bad_command: ");

	TEST_FORK
	{
		UnitTest_reopen(1, "stdout");
		TEST_EXPR(CMD_EXEC("/bin/echo", "echo  'a b\\'c' d\\ e\\"));
	}

	CHILD_EXIT(0);
	EXPECT_CONTENT("stdout", UnitTest_tpl("a b\\'c d e\n"));

	TEST_FORK
	{
		UnitTest_reopen(1, "stdout");
		TEST_EXPR(CMD_EXEC("/bin/echo", "echo @/a \\@/b"));
	}

	CHILD_EXIT(0);
	EXPECT_CONTENT("stdout", UnitTest_tpl("@/a \\@/b\n"));
}
