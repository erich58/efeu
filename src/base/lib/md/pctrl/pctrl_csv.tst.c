#include <EFEU/UnitTest.h>
#include <EFEU/pctrl.h>

static void render_table(const char *name, const char *mode)
{
	IO *io = io_pctrl(io_fopen(UnitTest_tmp(name), "w"), mode);
	io_ctrl(io, PCTRL_HEAD);
	io_ctrl(io, PCTRL_LEFT);
	io_puts("header \"text\"", io);
	io_ctrl(io, PCTRL_LINE);
	io_ctrl(io, PCTRL_EHEAD);

	io_ctrl(io, PCTRL_DATA, 5);

	io_ctrl(io, PCTRL_LEFT);
	io_puts("left", io);
	io_ctrl(io, PCTRL_VALUE);
	io_puts(" 1.1 ", io);
	io_ctrl(io, PCTRL_VALUE);
	io_puts("\t1.2\n", io);
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
	render_table("output", "csv");
	EXPECT_CONTENT("output",
		"\"header \"\"text\"\"\"\r\n"
		"\"left\"; 1.1 ; 1.2 \r\n"
		"\"right\";2.1;2.2\r\n"
		"\"center\";3.1;3.2\r\n"
	);

	render_table("output", "struct");
	EXPECT_CONTENT("output",
		"\t{ \"header \\\"text\\\"\" },\n"
		"\t{ \"left\",  1.1 ,  1.2  },\n"
		"\t{ \"right\", 2.1, 2.2 },\n"
		"\t{ \"center\", 3.1, 3.2 },\n"
	);
}
