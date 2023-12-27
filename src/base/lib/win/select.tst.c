#include <EFEU/UnitTest.h>
#include <EFEU/UnitTest_pty.h>
#include <EFEU/efwin.h>
#include <EFEU/parsub.h>
#include <EFEU/ioctrl.h>

TEST_SUITE
{
	int master = pty_open("output");

	setenv("TERM", "ansi", 1);
	setenv("COLUMNS", "20", 1);
	setenv("LINES", "10", 1);
	SetupWin();
	InitWin();
	UnitTest_write(1, "\n");

	char *list[] = { "first", "second" };

	UnitTest_write(master, "\033[A\033[B\033[C\033[D \t\b\n");
	EXPECT(WinSelect(CenterPos(5, 10), list, 2, 0) != NULL);
	UnitTest_write(1, "\n");

	EndWin();
	pty_close(master);
	PRINT_CONTENT("output");
}
