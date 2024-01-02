
#include <EFEU/UnitTest.h>
#include <EFEU/ioctrl.h>
#include <EFEU/pctrl.h>

static PCTRLDef dummy = {
	"dummy", "Dummy-Control", "|", "\n", "<FF>\n", NULL
};

TEST_DECL(list)
{
	IO *out = io_fileopen(UnitTest_tmp("list"), "w");
	pctrl_list(out);
	io_close(out);
	EXPECT_CONTENT("list",
		"(null)\tStandard output (space delimiter)\n"
		"ascii\tAscii output (tab delimiter)\n"
		"csv\tcsv-file\n"
		"data\tData without labels\n"
		"sc\tSC spreadsheet output\n"
		"struct\tList formatting\n"
		"tab\tLaTeX-table\n"
		"test\tTest output\n"
		"tex\tLaTeX-document\n"
	);

}

TEST_DECL(std)
{
	IO *io = io_fopen(UnitTest_tmp("output"), "w");
	EXPECT_PTR(io = io_pctrl(io, NULL));
	EXPECT_INT(io_ctrl(io, PCTRL_LINES), 0);
	EXPECT_INT(io_ctrl(io, PCTRL_COLUMNS), 0);
	EXPECT_INT(io_ctrl(io, PCTRL_YPOS), 1);
	EXPECT_INT(io_ctrl(io, PCTRL_XPOS), 0);

	io_ctrl(io, PCTRL_HEAD);
	io_ctrl(io, PCTRL_LEFT);
	io_puts("headline", io);
	io_ctrl(io, PCTRL_LINE);
	io_ctrl(io, PCTRL_EHEAD);
	EXPECT_INT(io_ctrl(io, PCTRL_YPOS), 2);
	EXPECT_INT(io_ctrl(io, PCTRL_XPOS), 0);

	io_ctrl(io, PCTRL_DATA);
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
		"headline\n"
		"left right\n"
		"123 \n"
		"\fcenter\n"
	);
}

TEST_DECL(dummy)
{
	pctrl_add(&dummy);
	IO *io = io_fopen(UnitTest_tmp("output"), "w");
	EXPECT_PTR(io = io_pctrl(io, "dummy"));
	EXPECT_INT(io_ctrl(io, PCTRL_LINES), -1);
	EXPECT_INT(io_ctrl(io, PCTRL_COLUMNS), -1);

	io_ctrl(io, PCTRL_HEAD);
	io_ctrl(io, PCTRL_LEFT);
	io_puts("headline", io);
	io_ctrl(io, PCTRL_LINE);
	io_ctrl(io, PCTRL_EHEAD);

	io_ctrl(io, PCTRL_DATA);
	io_ctrl(io, PCTRL_LEFT);
	io_puts("left", io);
	io_ctrl(io, PCTRL_RIGHT);
	io_puts("right", io);
	io_ctrl(io, PCTRL_VALUE);
	io_puts("123", io);
	io_ctrl(io, PCTRL_EMPTY);
	io_ctrl(io, PCTRL_LINE);
	io_ctrl(io, PCTRL_PAGE);
	io_ctrl(io, PCTRL_CENTER);
	io_puts("center", io);
	io_ctrl(io, PCTRL_LINE);
	EXPECT_INT(io_ctrl(io, IO_FLUSH), 0);
	io_close(io);

	EXPECT_CONTENT("output",
		"\n"
		"|||\n"
		"<FF>\n"
		"\n"
	);
}

TEST_SUITE
{
	TEST_RUN(list);
	TEST_RUN(std);
	TEST_RUN(dummy);
}
