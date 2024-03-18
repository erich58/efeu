#include <EFEU/UnitTest.h>
#include <EFEU/pctrl.h>

TEST_SUITE
{
	IO *io;

	EXPECT_PTR(io = io_pctrl(io_fopen(UnitTest_tmp("output"), "w"), "tab"));
	io_ctrl(io, PCTRL_DATA, 5);
	io_ctrl(io, PCTRL_EDATA);
	io_close(io);
	EXPECT_FILE("output",
		"\\small\n"
		"\n"
		"\\begin{tabular}{|l|*{5}{r|}}\n"
		"\\hline\n"
		"\\hline\n"
		"\\end{tabular}\n"
	);

	EXPECT_PTR(io = io_pctrl(io_fopen(UnitTest_tmp("output"), "w"), "tex"));

	EXPECT_INT(io_ctrl(io, PCTRL_FSIZE, 10), 10);
	EXPECT_INT(io_ctrl(io, PCTRL_PGFMT, "a4"), 96);
	EXPECT_INT(io_ctrl(io, PCTRL_LINES), 56);
	EXPECT_INT(io_ctrl(io, PCTRL_COLUMNS), 96);

	io_ctrl(io, PCTRL_HEAD);
	io_ctrl(io, PCTRL_LEFT);
	io_puts("headline % $ # _ & { }\t\n", io);
	io_puts("^ ~ | [ ] < > \n", io);
	io_puts("\"abc\" \\ * \001 \200 ", io);
	io_ctrl(io, PCTRL_LINE);
	io_ctrl(io, PCTRL_EHEAD);

	io_ctrl(io, PCTRL_DATA, 5);
	io_ctrl(io, PCTRL_LEFT);
	io_puts("left", io);
	io_ctrl(io, PCTRL_RIGHT);
	io_puts("right", io);
	io_ctrl(io, PCTRL_LINE);
	io_ctrl(io, PCTRL_VALUE);
	io_puts("123.45", io);
	io_ctrl(io, PCTRL_EMPTY);
	io_ctrl(io, PCTRL_LINE);
	io_ctrl(io, PCTRL_CENTER);
	io_puts("center", io);
	io_ctrl(io, PCTRL_VALUE);
	io_puts("1", io);
	io_ctrl(io, PCTRL_LINE);
	io_ctrl(io, PCTRL_EDATA);
	io_close(io);
	EXPECT_FILE("output",
		"\\documentclass[11pt,a4paper]{article}\n"
		"\\usepackage[latin1]{inputenc}\n"
		"\\usepackage{german}\n"
		"\\dateaustrian\n"
		"\\topmargin=19.0000mm\n"
		"\\oddsidemargin=16.0000mm\n"
		"\\textwidth=170.000mm\n"
		"\\textheight=245.000mm\n"
		"\\voffset=-25mm\\hoffset=-25mm\n"
		"\\headheight=0mm\\headsep=0mm\n"
		"\\evensidemargin=\\oddsidemargin\n"
		"\\footskip=15mm\n"
		"\\parskip\\baselineskip\n"
		"\\begin{document}\n"
		"\\raggedright\n"
		"\\small\n"
		"headline \\% \\$ \\# \\_ \\& \\{ \\}\t~\\\\\n"
		"\\^~ \\~~ $|$ $[$ $]$ $<$ $>$ ~\\\\\n"
		"\"`abc\"' $\\backslash$ $\\ast$ $\\bullet$ $\\bullet$ ~\\\\\n"
		"\n"
		"\\begin{tabular}{|l|*{5}{r|}}\n"
		"\\hline\n"
		"left & right \\\\\n"
		"123.45 &  \\\\\n"
		"center & 1 \\\\\n"
		"\\hline\n"
		"\\end{tabular}\n"
		"\\end{document}\n"
	);

	if	(UnitTest_argc > 1)
	{
		char buf[1024];
		snprintf(buf, sizeof buf, "cp @/output %s", UnitTest_argv[1]); 
		UnitTest_system(buf);
	}
}
