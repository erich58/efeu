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
	UnitTest_write(master, "\033x");
	EXPECT(WinQuestion(NULL, NULL, NULL, NULL) == 0);
	EXPECT(getch() == 'x');

	UnitTest_write(1, "\n#2\n");
	UnitTest_write(master, "\r");
	EXPECT(WinQuestion(NULL, "Frage", "JA", "NEIN") == 1);

	UnitTest_write(1, "\n#3\n");
	UnitTest_write(master, "x\033[D\033[C \t\r");
	EXPECT(WinQuestion(NULL, NULL, NULL, NULL) == 2);
	UnitTest_write(1, "\n");

	UnitTest_write(1, "\n#END\n");
	EndWin();
	pty_close(master);
	PRINT_CONTENT("output");
}
