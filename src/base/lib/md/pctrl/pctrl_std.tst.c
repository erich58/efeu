#include <EFEU/UnitTest.h>
#include <EFEU/pctrl.h>

static void render_table(const char *name, const char *mode)
{
	IO *io = io_pctrl(io_fopen(UnitTest_tmp(name), "w"), mode);
	io_ctrl(io, PCTRL_HEAD);
	io_ctrl(io, PCTRL_LEFT);
	io_puts("header x\tx\nx", io);
	io_ctrl(io, PCTRL_LINE);
	io_ctrl(io, PCTRL_EHEAD);

	io_ctrl(io, PCTRL_DATA, 5);

	io_ctrl(io, PCTRL_LEFT);
	io_puts("left", io);
	io_ctrl(io, PCTRL_VALUE);
	io_puts("1.1", io);
	io_ctrl(io, PCTRL_VALUE);
	io_puts("1.2", io);
	io_ctrl(io, PCTRL_LINE);

	io_ctrl(io, PCTRL_RIGHT);
	io_puts("right", io);
	io_ctrl(io, PCTRL_VALUE);
	io_puts("2.1", io);
	io_ctrl(io, PCTRL_VALUE);
	io_puts("2.2", io);
	io_ctrl(io, PCTRL_LINE);

	io_ctrl(io, PCTRL_CENTER);
	io_puts("center", io);
	io_ctrl(io, PCTRL_VALUE);
	io_puts("3.1", io);
	io_ctrl(io, PCTRL_VALUE);
	io_puts("3.2", io);
	io_ctrl(io, PCTRL_LINE);

	io_ctrl(io, PCTRL_EDATA);
	io_close(io);
}

TEST_SUITE
{
	render_table("output", NULL);
	EXPECT_FILE("output",
		"header x x x\n"
		"left 1.1 1.2\n"
		"right 2.1 2.2\n"
		"center 3.1 3.2\n"
	);
	render_table("output", "ascii");
	EXPECT_FILE("output",
		"header x x x\n"
		"left\t1.1\t1.2\n"
		"right\t2.1\t2.2\n"
		"center\t3.1\t3.2\n"
	);
	render_table("output", "data");
	EXPECT_FILE("output",
		"1.1, 1.2\n"
		"2.1, 2.2\n"
		"3.1, 3.2\n"
	);
	PrintListDelim = "|";
	render_table("output", "data");
	EXPECT_FILE("output",
		"1.1|1.2\n"
		"2.1|2.2\n"
		"3.1|3.2\n"
	);
}
