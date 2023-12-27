#include <EFEU/UnitTest.h>
#include <EFEU/UnitTest_pty.h>
#include <EFEU/efwin.h>
#include <EFEU/parsub.h>
#include <EFEU/ioctrl.h>

TEST_SUITE
{
	int master = pty_open("output");

	setenv("TERM", "ansi", 1);
	SetupWin();
	InitWin();

	UnitTest_write(1, "\n#1\n");
	UnitTest_write(master, "x");
	WinConfirm(NULL, NULL);

	UnitTest_write(1, "\n#2\n");
	UnitTest_write(master, "\r");
	WinConfirm(NULL, "first line\nsecond line\n");

	UnitTest_write(1, "\n#END\n");
	EndWin();
	pty_close(master);
	PRINT_CONTENT("output");
}
