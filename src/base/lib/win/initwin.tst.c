#include <EFEU/UnitTest.h>
#include <EFEU/UnitTest_pty.h>
#include <EFEU/efwin.h>
#include <EFEU/parsub.h>
#include <EFEU/ioctrl.h>

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
	int master = pty_open("output");

	callproc_init = base_init;
	callproc_exit = base_exit;
	setenv("TERM", "ansi", 1);

	SetupWin();
	EXPECT_INT(callproc("true"), 0);
	EXPECT_INT(init_count, 1);
	EXPECT_INT(exit_count, 1);
	InitWin();
	InitWin();
	UnitTest_write(1, "\n*callproc\n");
	EXPECT_INT(callproc("echo ABC"), 0);
	EXPECT_INT(init_count, 2);
	EXPECT_INT(exit_count, 2);

	UnitTest_write(1, "\n*END\n");
	EndWin();

	pty_close(master);
	PRINT_CONTENT("output");
}
