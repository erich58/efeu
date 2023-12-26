#include <EFEU/UnitTest.h>
#include <EFEU/efwin.h>
#include <EFEU/parsub.h>
#include <EFEU/ioctrl.h>
#include <pty.h>

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

static void write_string(int fd, const char *str)
{
	write(fd, str, strlen(str));
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

	dup2(slave, 0);
	dup2(slave, 1);
	close(slave);
	EXPECT(isatty(0) == 1);
	EXPECT(isatty(1) == 1);

	callproc_init = base_init;
	callproc_exit = base_exit;
	setenv("TERM", "ansi", 1);
	setenv("COLUMNS", "20", 1);
	setenv("LINES", "10", 1);
	SetupWin();
	EXPECT_INT(callproc("true"), 0);
	EXPECT_INT(init_count, 1);
	EXPECT_INT(exit_count, 1);
	write_string(1, "BEG\n");
	write_string(1, "InitWin{\n");
	InitWin();
	InitWin();
	write_string(1, "\n}\n");
	write_string(1, "callproc{\n");
	EXPECT_INT(callproc("echo ABC"), 0);
	write_string(1, "\n}\n");
	EXPECT_INT(init_count, 2);
	EXPECT_INT(exit_count, 2);

	WinSize *s = CheckSize(NULL);
	EXPECT_INT(s->height, 10);
	EXPECT_INT(s->width, 20);
	EXPECT_INT(s->flags, W_TOP|W_LEFT);
	EXPECT_INT(s->y, 0);
	EXPECT_INT(s->x, 0);

	s = Frame(s);
	EXPECT_INT(s->height, 10);
	EXPECT_INT(s->width, 20);
	EXPECT_INT(s->flags, W_TOP|W_LEFT|W_FRAME);
	EXPECT_INT(s->y, 0);
	EXPECT_INT(s->x, 0);

	s = WindowSize(5, 7, W_CENTER, 2, 3);
	EXPECT_INT(s->height, 5);
	EXPECT_INT(s->width, 7);
	EXPECT_INT(s->flags, W_CENTER);
	EXPECT_INT(s->y, 2);
	EXPECT_INT(s->x, 3);

	s = CenterPos(3, 5);
	EXPECT_INT(s->height, 3);
	EXPECT_INT(s->width, 5);
	EXPECT_INT(s->flags, W_CENTER);
	EXPECT_INT(s->y, 5);
	EXPECT_INT(s->x, 10);

	s = Rand48Pos(10, 20);
	EXPECT_INT(s->height, 10);
	EXPECT_INT(s->width, 20);
	EXPECT_INT(s->flags, W_CENTER);
	EXPECT_INT(s->y, 5);
	EXPECT_INT(s->x, 10);

	write_string(1, "NewWindow{\n");
	WINDOW *win = NewWindow(NULL);
	write_string(1, "\n}\n");
	write_string(1, "io_winout{\n");
	IO *io = io_winout(win);
	write_string(1, "}\n");
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
	write_string(1, "\n}\n");
	DelWindow(win);

	write_string(1, "io_winopen{\n");
	io = io_winopen(CenterPos(5, 10));
	EXPECT_INT(io_ctrl(io, IO_ERROR), EOF);
	io_puts("12345", io);
	io_close(io);
	write_string(1, "\n}\n");

	write_string(1, "StippleBackground{\n");
	win = StippleBackground();
	write_string(1, "\n}\n");
	DelWindow(win);

	write_string(1, "NewWindow{\n");
	WINDOW *w1 = NewWindow(CenterPos(5, 10));
	write_string(1, "\n}\n");

	write_string(1, "NewWindow(Frame){\n");
	WINDOW *w2 = NewWindow(Frame(CenterPos(5, 10)));
	write_string(1, "\n}\n");
	RefreshAll();

	DelWindow(w1);
	DelWindow(w2);

	write_string(master, "\n");
	write_string(1, "WinConfirm{\n");
	WinConfirm(NULL, NULL);
	write_string(1, "\n}\n");

	write_string(master, "\n");
	write_string(1, "WinConfirm{\n");
	WinConfirm(NULL, "first line\nsecond line\n");
	write_string(1, "\n}\n");

	write_string(master, "\033[C\033[D \n");
	write_string(1, "WinQuestion{\n");
	WinQuestion(NULL, NULL, NULL, NULL);
	write_string(1, "\n}\n");

	char *list[] = { "first", "second" };

	write_string(master, "\033[A\033[B\033[C\033[D \t\b\n");
	write_string(1, "WinSelect{\n");
	WinSelect(CenterPos(5, 10), list, 2, 0);
	write_string(1, "\n}\n");

	write_string(1, "EndWin\n");
	EndWin();
	write_string(1, "\n");
	write_string(1, "END\n");

	close(master);
	UnitTest_stdin(NULL, NULL);
	UnitTest_stdout(NULL);

	CHILD_EXIT(0);
	PRINT_CONTENT("output");
}
