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
	WinSize *s;
	IO *io;
	UnitTest_write(1, "\nNewWindow\n");
	EXPECT_PTR(win = NewWindow(NULL));

	UnitTest_write(1, "\n*io_winout\n");
	EXPECT_PTR(io = io_winout(win));
	EXPECT_INT(io_ctrl(io, IO_ERROR), EOF);
	EXPECT_INT(io_ctrl(io, IO_COLUMNS), 20);
	EXPECT_INT(io_ctrl(io, IO_LINES), 10);
	io_puts("a\n123", io);
	s = CurrentPos(win, 5, 10, 0);
	EXPECT_INT(s->height, 5);
	EXPECT_INT(s->width, 10);
	EXPECT_INT(s->flags, W_CENTER);
	EXPECT_INT(s->y, 1);
	EXPECT_INT(s->x, 3);
	io_puts("\n", io);
	io_ctrl(io, IO_BOLD_FONT);
	io_puts("B", io);
	io_ctrl(io, IO_ITALIC_FONT);
	io_puts("I", io);
	io_ctrl(io, IO_ROMAN_FONT);
	io_puts("R", io);
	io_close(io);
	DelWindow(win);

	UnitTest_write(1, "\n*io_winopen\n");
	EXPECT_PTR(io = io_winopen(CenterPos(5, 10)));
	EXPECT_INT(io_ctrl(io, IO_ERROR), EOF);
	io_puts("12345", io);
	io_close(io);

	UnitTest_write(1, "\n#END\n");
	EndWin();
	pty_close(master);
	PRINT_CONTENT("output");
}
