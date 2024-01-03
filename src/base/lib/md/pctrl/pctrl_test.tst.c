#include <EFEU/UnitTest.h>
#include <EFEU/pctrl.h>

TEST_SUITE
{
	IO *io = io_fopen(UnitTest_tmp("output"), "w");
	io = io_pctrl(io, "test");
	EXPECT(io != 0);
	EXPECT_INT(io_ctrl(io, PCTRL_LINES), -1);
	EXPECT_INT(io_ctrl(io, PCTRL_COLUMNS), -1);
	EXPECT_INT(io_ctrl(io, PCTRL_YPOS), 1);
	EXPECT_INT(io_ctrl(io, PCTRL_XPOS), 0);

	io_ctrl(io, PCTRL_HEAD);
	io_ctrl(io, PCTRL_LEFT);
	io_puts("headline", io);
	io_ctrl(io, PCTRL_LINE);
	io_ctrl(io, PCTRL_EHEAD);
	EXPECT_INT(io_ctrl(io, PCTRL_YPOS), 2);
	EXPECT_INT(io_ctrl(io, PCTRL_XPOS), 0);

	io_ctrl(io, PCTRL_DATA, 5);
	io_ctrl(io, PCTRL_LEFT);
	io_puts("left", io);
	EXPECT_INT(io_ctrl(io, PCTRL_YPOS), 2);
	EXPECT_INT(io_ctrl(io, PCTRL_XPOS), 1);
	io_ctrl(io, PCTRL_RIGHT);
	io_puts("right", io);
	EXPECT_INT(io_ctrl(io, PCTRL_YPOS), 2);
	EXPECT_INT(io_ctrl(io, PCTRL_XPOS), 2);
	io_ctrl(io, PCTRL_LINE);
	EXPECT_INT(io_ctrl(io, PCTRL_YPOS), 3);
	EXPECT_INT(io_ctrl(io, PCTRL_XPOS), 0);
	io_ctrl(io, PCTRL_VALUE);
	io_puts("123", io);
	EXPECT_INT(io_ctrl(io, PCTRL_YPOS), 3);
	EXPECT_INT(io_ctrl(io, PCTRL_XPOS), 1);
	io_ctrl(io, PCTRL_EMPTY);
	EXPECT_INT(io_ctrl(io, PCTRL_YPOS), 3);
	EXPECT_INT(io_ctrl(io, PCTRL_XPOS), 2);
	io_ctrl(io, PCTRL_LINE);
	EXPECT_INT(io_ctrl(io, PCTRL_YPOS), 4);
	EXPECT_INT(io_ctrl(io, PCTRL_XPOS), 0);
	io_ctrl(io, PCTRL_PAGE);
	EXPECT_INT(io_ctrl(io, PCTRL_YPOS), 1);
	EXPECT_INT(io_ctrl(io, PCTRL_XPOS), 0);
	io_ctrl(io, PCTRL_CENTER);
	io_puts("center", io);
	EXPECT_INT(io_ctrl(io, PCTRL_YPOS), 1);
	EXPECT_INT(io_ctrl(io, PCTRL_XPOS), 1);
	io_ctrl(io, PCTRL_LINE);
	EXPECT_INT(io_ctrl(io, PCTRL_YPOS), 2);
	EXPECT_INT(io_ctrl(io, PCTRL_XPOS), 0);
	io_ctrl(io, PCTRL_EDATA);
	io_close(io);
	EXPECT_CONTENT("output",
		"BEGIN\n"
		"<7022>\n"
		"<7021>\n"
		"HEAD\n"
		"<1,1>LEFT(headline)<NL>\n"
		"EHEAD\n"
		"DATA(5)\n"
		"<2,1>LEFT(left), <2,2>RIGHT(right)<NL>\n"
		"<3,1>VAL(123), <3,2>EMPTY()<NL>\n"
		"<FF>\f<1,1>CENTER(center)<NL>\n"
		"EDATA\n"
		"END\n"
	);
}
