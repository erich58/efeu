#include <EFEU/UnitTest.h>
#include <EFEU/pctrl.h>

TEST_SUITE
{
	IO *io = io_fopen(UnitTest_tmp("output"), "w");
	io = io_pctrl(io, "sc");
	EXPECT(io != 0);

	io_ctrl(io, PCTRL_HEAD);
	io_ctrl(io, PCTRL_LEFT);
	io_puts("headline \"text\"", io);
	io_ctrl(io, PCTRL_LINE);
	io_ctrl(io, PCTRL_EHEAD);

	io_ctrl(io, PCTRL_DATA);
	io_ctrl(io, PCTRL_LEFT);
	io_puts("left", io);
	io_ctrl(io, PCTRL_RIGHT);
	io_puts("right", io);
	io_ctrl(io, PCTRL_LINE);
	io_ctrl(io, PCTRL_VALUE);
	io_puts("123.45\t\n", io);
	io_ctrl(io, PCTRL_EMPTY);
	io_ctrl(io, PCTRL_LINE);
	io_ctrl(io, PCTRL_CENTER);
	io_puts("center", io);
	io_ctrl(io, PCTRL_LINE);
	io_ctrl(io, PCTRL_EDATA);
	io_close(io);
	EXPECT_FILE("output",
		"# Spreadsheet Calculator table\n"
		"\n"
		"leftstring A0 = \"headline \\\"text\\\"\"\n"
		"leftstring A1 = \"left\"\n"
		"rightstring B1 = \"right\"\n"
		"let A2 = 123.45  \n"
		"\n"
		"label A3 = \"center\"\n"
		"format A 6 0 0\n"
		"format B 9 4 0\n"
		"goto A0\n"
	);
}
