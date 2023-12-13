#include <EFEU/UnitTest.h>
#include <EFEU/ioctrl.h>
#include <EFEU/Readline.h>
#include <EFEU/parsub.h>
#include <pty.h>

#include "utest/mstr.tst.h"
#include <EFEU/rl_config.h>

static int c_id(IO *in, IO *out, void *arg)
{
	return io_puts("ID", out);
}

/*	Mock readline
*/

static const char *input = "";

char *readline(const char *prompt)
{
	UnitTest_info(__FILE__, __LINE__, "INFO", "# %s(\"%s\") -> %s",
		__func__, prompt, UnitTest_qstr(input));
	char *p = input ? strdup(input) : NULL;
	input = "";
	return p;
}

TEST_SUITE
{
	psubfunc('!', c_id, NULL);
	UnitTest_mkcmd("vi",
		"#!/bin/sh\n"
		"cp $1 $0.out\n"
		"echo edited > $1\n"
	);
	UnitTest_mkcmd("filter",
		"#!/bin/sh\n"
		"cat > $0.out\n"
	);

	IO *io;
	int master;
	int slave;
	UnitTest_stdin("in", "");
	UnitTest_stdout("out");
	EXPECT(isatty(0) == 0);
	EXPECT(isatty(1) == 0);
	SetupReadline();
	EXPECT_PTR(io = io_interact(NULL, NULL));
	EXPECT_MSTR(rd_ident(io), "<stdin,stdout>");
	io_close(io);

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

	close(master);
	dup2(slave, 0);
	dup2(slave, 1);
	close(slave);
	EXPECT(isatty(0) == 1);
	EXPECT(isatty(1) == 1);

	SetupReadline();
	TEST_FILE("History",
		"First\n"
		"Second\n"
		"Last\n"
	);
	EXPECT_PTR(io = io_interact(NULL, UnitTest_tmp("History")));
	io_puts("START\n", io);
	EXPECT_INT(io_ctrl(io, IO_LINE), 1);
	EXPECT_INT(io_ctrl(io, IO_ERROR), 0);
	EXPECT_INT(io_ctrl(io, IO_REWIND), EOF);
	EXPECT_INT(io_ctrl(io, RLCTRL), 0);
	EXPECT_INT(io_ctrl(io, RLCTRL_HSIZE, 3), 0);
	EXPECT_INT(io_ctrl(io, IO_PROMPT, NULL), EOF);
	EXPECT_MSTR(rd_ident(io), "<readline>:1");

	input = "Data";
	EXPECT_MSTR(io_getline(io, NULL, '\n'), "Data");
	char *prompt = "Prompt";
	EXPECT_INT(io_ctrl(io, IO_PROMPT, &prompt), 0);
	EXPECT_STR(prompt, NULL);
	EXPECT_MSTR(io_getline(io, NULL, '\n'), "");
	EXPECT_INT(io_ctrl(io, IO_PROMPT, &prompt), 0);
	EXPECT_STR(prompt, "Prompt");

	EXPECT_INT(io_ctrl(io, IO_PROTECT, 1), 1);
	EXPECT_MSTR(io_getline(io, NULL, '\n'), "");
	EXPECT_INT(io_ctrl(io, IO_PROTECT, 0), 0);
	EXPECT_MSTR(io_getline(io, NULL, '\n'), "");
	EXPECT_INT(io_ctrl(io, IO_LINEMARK), 0);
	EXPECT_MSTR(io_getline(io, NULL, '\n'), "");
	EXPECT_MSTR(io_getline(io, NULL, '\n'), "");

	TEST_EXPR(input = "!r");
	EXPECT_MSTR(io_getline(io, NULL, '\n'), "Data");
	EXPECT_INT(io_ctrl(io, RLCTRL_KEY, '@'), 0);
	TEST_EXPR(input = "@r");
	EXPECT_MSTR(io_getline(io, NULL, '\n'), "Data");
	EXPECT_INT(io_ctrl(io, RLCTRL_KEY, '!'), 0);

	TEST_EXPR(input = "!h");
	EXPECT_MSTR(io_getline(io, NULL, '\n'), "");

	TEST_EXPR(input = "!h 3");
	EXPECT_MSTR(io_getline(io, NULL, '\n'), "");

	TEST_EXPR(input = "!h 1 3 ");
	EXPECT_MSTR(io_getline(io, NULL, '\n'), "");

	TEST_EXPR(input = "!h 1 3 |filter");
	EXPECT_MSTR(io_getline(io, NULL, '\n'), "");
	EXPECT_CONTENT("bin/filter.out",
		"1: First\n"
		"2: Second\n"
		"3: Last\n"
	);

	TEST_EXPR(input = "!x");
	EXPECT_MSTR(io_getline(io, NULL, '\n'), "!x");

	TEST_EXPR(input = "!r 3");
	EXPECT_MSTR(io_getline(io, NULL, '\n'), "Last");

	TEST_EXPR(input = "!fc 1 3");
	EXPECT_MSTR(io_getline(io, NULL, '\n'), "edited");
	EXPECT_CONTENT("bin/vi.out",
		"First\n"
		"Second\n"
		"Last\n"
	);

	TEST_EXPR(input = "!eof");
	EXPECT_MSTR(io_getline(io, NULL, '\n'), NULL);
	io_close(io);
	PRINT_CONTENT("History");

	EXPECT_PTR(io = io_interact(NULL, UnitTest_tmp("History")));
	TEST_EXPR(input = NULL);
	EXPECT_MSTR(io_getline(io, NULL, '\n'), NULL);

	io_puts("END\n", io);
	io_close(io);
	UnitTest_stdin(NULL, NULL);
	UnitTest_stdout(NULL);

	CHILD_EXIT(0);
	PRINT_CONTENT("output");
}
