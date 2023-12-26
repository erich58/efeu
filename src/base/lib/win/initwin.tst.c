#include <EFEU/UnitTest.h>
#include <EFEU/efwin.h>
#include <EFEU/parsub.h>
#include <pty.h>

//#include "utest/mstr.tst.h"
//#include <EFEU/rl_config.h>

extern void (*callproc_init) (void);
extern void (*callproc_exit) (void);

static int init_count = 0;

static void base_init (void)
{
	init_count++;
}

static int exit_count = 0;

static void base_exit (void)
{
	exit_count++;
}

TEST_SUITE
{
	UnitTest_stdin("in", "");
	UnitTest_stdout("out");

	int master;
	int slave;
	ASSERT(openpty(&master, &slave, NULL, NULL, NULL) == 0);

	TEST_FORK
	{
		char buf[64];
		ssize_t n;
		close(slave);
		IO *out = io_fopen(UnitTest_tmp("output"), "w");

		while ((n = read(master, buf, sizeof buf)) > 0)
			io_write(out, buf, n);

		io_close(out);
		close(master);
		TEST_EXIT;
	}

	close(master);
	dup2(slave, 0);
	dup2(slave, 1);
	close(slave);
	EXPECT(isatty(0) == 1);
	EXPECT(isatty(1) == 1);

	callproc_init = base_init;
	callproc_exit = base_exit;
	SetupWin();
	EXPECT_INT(callproc("true"), 0);
	EXPECT_INT(init_count, 1);
	EXPECT_INT(exit_count, 1);
	write(1, "BEG\n", 4);
	InitWin();
	InitWin();
	write(1, "\n{\n", 3);
	EXPECT_INT(callproc("true"), 0);
	write(1, "\n}\n", 3);
	EXPECT_INT(init_count, 2);
	EXPECT_INT(exit_count, 2);
	EndWin();
	write(1, "\nEND\n", 5);

	UnitTest_stdin(NULL, NULL);
	UnitTest_stdout(NULL);

	CHILD_EXIT(0);
	PRINT_CONTENT("output");
}
