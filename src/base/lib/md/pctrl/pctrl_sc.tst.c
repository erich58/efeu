#include <EFEU/UnitTest.h>
#include <EFEU/pctrl.h>

TEST_SUITE
{
	IO *io;
	EXPECT_PTR(io = io_pctrl(io_fopen(UnitTest_tmp("output"), "w"), "sc"));

	io_ctrl(io, PCTRL_HEAD);
	io_ctrl(io, PCTRL_LEFT);
	io_puts("headline \"text\"", io);
	io_ctrl(io, PCTRL_LINE);
	io_ctrl(io, PCTRL_EHEAD);

	io_ctrl(io, PCTRL_DATA, 5);
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

	EXPECT_PTR(io = io_pctrl(io_fopen(UnitTest_tmp("output"), "w"), "sc"));
	io_ctrl(io, PCTRL_DATA, 5);
	io_ctrl(io, PCTRL_LEFT);
	io_puts("label", io);
	io_ctrl(io, PCTRL_RIGHT);
	io_puts("column", io);
	io_ctrl(io, PCTRL_LINE);
	io_ctrl(io, PCTRL_LEFT);
	io_puts("line", io);

	for (int i = 0; i < 25; ++i)
		io_ctrl(io, PCTRL_EMPTY);

	io_ctrl(io, PCTRL_VALUE);
	io_puts("1", io);
	io_ctrl(io, PCTRL_LINE);
	io_ctrl(io, PCTRL_EDATA);

	io_close(io);
	LMATCH_STR(UnitTest_content("output"),
		"leftstring A0 = \"label\"\n"
		"rightstring B0 = \"column\"\n"
		"leftstring A1 = \"line\"\n"
		"let AA1 = 1\n"
		"format A 5 0 0\n"
		"format B 7 0 0\n"
		"format AA 7 0 0\n"
	);
}
