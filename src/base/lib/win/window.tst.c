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

	UnitTest_write(1, "\n#1\n");
	WINDOW *w1 = NewWindow(CenterPos(5, 10));

	UnitTest_write(1, "\n#2\n");
	WINDOW *w2 = NewWindow(Frame(CenterPos(5, 10)));

	UnitTest_write(1, "\n#Refresh\n");
	RefreshAll();
	DelWindow(w1);
	DelWindow(w2);
	RefreshAll();

	UnitTest_write(1, "\n#END\n");
	EndWin();
	pty_close(master);
	PRINT_CONTENT("output");
}
