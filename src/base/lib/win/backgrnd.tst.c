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

	WINDOW *win;
	UnitTest_write(1, "\n#StippleBackground\n");
	EXPECT_PTR(win = StippleBackground());
	DelWindow(win);

	UnitTest_write(1, "\n#END\n");
	EndWin();
	pty_close(master);
	PRINT_CONTENT("output");
}
