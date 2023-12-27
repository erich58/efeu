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
	UnitTest_write(1, "\n*BEG\n");

	WinSize *s;
	EXPECT_PTR(s = CheckSize(NULL));
	EXPECT_INT(s->height, 10);
	EXPECT_INT(s->width, 20);
	EXPECT_INT(s->flags, W_TOP|W_LEFT);
	EXPECT_INT(s->y, 0);
	EXPECT_INT(s->x, 0);

	EXPECT_PTR(s = Frame(s));
	EXPECT_INT(s->height, 10);
	EXPECT_INT(s->width, 20);
	EXPECT_INT(s->flags, W_TOP|W_LEFT|W_FRAME);
	EXPECT_INT(s->y, 0);
	EXPECT_INT(s->x, 0);

	EXPECT_PTR(s = WindowSize(5, 7, W_CENTER, 2, 3));
	EXPECT_INT(s->height, 5);
	EXPECT_INT(s->width, 7);
	EXPECT_INT(s->flags, W_CENTER);
	EXPECT_INT(s->y, 2);
	EXPECT_INT(s->x, 3);

	EXPECT_PTR(s = CenterPos(3, 5));
	EXPECT_INT(s->height, 3);
	EXPECT_INT(s->width, 5);
	EXPECT_INT(s->flags, W_CENTER);
	EXPECT_INT(s->y, 5);
	EXPECT_INT(s->x, 10);

	EXPECT_PTR(s = Rand48Pos(10, 20));
	EXPECT_INT(s->height, 10);
	EXPECT_INT(s->width, 20);
	EXPECT_INT(s->flags, W_CENTER);
	EXPECT_INT(s->y, 5);
	EXPECT_INT(s->x, 10);

	UnitTest_write(1, "\n*END\n");
	EndWin();

	pty_close(master);
	PRINT_CONTENT("output");
}
