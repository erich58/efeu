#include <EFEU/UnitTest.h>
#include <EFEU/ioctrl.h>
#include <pty.h>

#include "utest/mstr.tst.h"

TEST_DECL(null)
{
	EXPECT_MSTR(rd_ident(NULL), NULL);
	EXPECT(io_getc(NULL) == EOF);
	EXPECT(io_puts("test\n", NULL) == 0);

	EXPECT(io_ctrl(NULL, IO_CLOSE) == EOF);
}

TEST_DECL(ionull)
{
	EXPECT_MSTR(rd_ident(ionull), "<null>");
	EXPECT(io_getc(ionull) == EOF);
	EXPECT(io_puts("test\n", ionull) == 5);

	EXPECT(io_ctrl(ionull, IO_CLOSE) == EOF);
	EXPECT(io_ctrl(ionull, IO_REWIND) == 0);
}

TEST_DECL(iostd)
{
	char buf[8];

	EXPECT(iostd == io_batch());
	EXPECT(iomsg == io_batch());
	EXPECT_MSTR(rd_ident(iostd), "<stdin,stdout>");

	UnitTest_stdin("stdin", "data\n");
	EXPECT_STR(io_gets(buf, sizeof buf, iostd), "data\n");
	UnitTest_stdin(NULL, NULL);

	UnitTest_stdout("stdout");
	EXPECT(io_puts("test\n", iostd) == 5);
	UnitTest_stdout(NULL);
	EXPECT_CONTENT("stdout", "test\n");

	EXPECT(io_ctrl(iostd, IO_REWIND) == EOF);
	EXPECT(io_ctrl(iostd, IO_FDIN) == 0);
	EXPECT(io_ctrl(iostd, IO_FDOUT) == 1);
	EXPECT(io_ctrl(iostd, IO_FLUSH) == 0);
	EXPECT(io_ctrl(iostd, IO_CLOSE) == 0);
}

TEST_DECL(ioerr)
{
	EXPECT_MSTR(rd_ident(ioerr), "<stderr>");

	EXPECT(io_getc(ioerr) == EOF);

	UnitTest_stderr("stderr");
	EXPECT(io_puts("test\n", ioerr) == 5);
	UnitTest_stderr(NULL);
	EXPECT_CONTENT("stderr", "test\n");

	EXPECT(io_ctrl(iostd, IO_REWIND) == EOF);
	EXPECT(io_ctrl(ioerr, IO_FDIN) == EOF);
	EXPECT(io_ctrl(ioerr, IO_FDOUT) == 2);
	EXPECT(io_ctrl(ioerr, IO_FLUSH) == 0);
	EXPECT(io_ctrl(ioerr, IO_CLOSE) == 0);
}

static IO *test_interact (const char *prompt, const char *hist)
{
	return ioerr;
}

TEST_DECL(interact)
{
	UnitTest_stdin("stdin", NULL);
	EXPECT(io_interact("P", "H") == iostd);
	_interact_open = test_interact;
	EXPECT(io_interact("P", "H") == ioerr);
	_interact_open = NULL;
	UnitTest_stdin(NULL, NULL);

	int master;
	int slave;
	ASSERT(openpty(&master, &slave,NULL, NULL, NULL) == 0);

	TEST_FORK
	{
		IO *io;
		dup2(slave, 0);
		dup2(slave, 1);
		close(master);
		close(slave);

		io = io_interact(NULL, NULL);
		UnitTest_stdout("stdout");
		EXPECT_MSTR(rd_ident(io), "<interact>:1");

		int c;
		int last = '\n';
		const char *p;
		int quote = 0;

		EXPECT(io_ctrl(io, IO_PROMPT, NULL) == EOF);
		EXPECT(io_ctrl(io, IO_ERROR) == 0);

		while ((c = io_getc(io)) != EOF)
		{
			if	(last == '\n')
				io_printf(io, "\t%2d ", io_ctrl(io, IO_LINE));

			io_putc(c, io);

			if	(c == '\'')
				io_submode(io, (quote = !quote));
			else if	(c == '+')
				p = io_prompt(io, "# ");
			else if	(c == '-')
				io_prompt(io, p);

			if	(c == '\n' && last == '\\')
				io_linemark(io);

			last = c;
		}

		io_close(io);
		exit(0);
	}

	close(slave);
	FILE *file = fdopen(master, "w");
	fputs("test\n", file);
	fputs("wrap\\\nline\n\n", file);
	fputs("string 'abc\n", file);
	fputs("def\n", file);
	fputs("xyz'\n\n", file);
	fputs("line\n", file);
	fputs("+\n", file);
	fputs("line\n", file);
	fputs("-\n", file);
	fputs("line\n", file);
	putc(4, file);
	fflush(file);
	CHILD_EXIT(0);
	fclose(file);
	EXPECT_CONTENT("stdout",
		"!: \t 1 test\n"
		"!: \t 2 wrap\\\n"
		"> \t 3 line\n"
		"!: \t 4 \n"
		"!: \t 5 string 'abc\n"
		"> \t 6 def\n"
		"> \t 7 xyz'\n"
		"!: \t 8 \n"
		"!: \t 9 line\n"
		"!: \t10 +\n"
		"# \t11 line\n"
		"# \t12 -\n"
		"!: \t13 line\n"
		"!: \n"
	);
}

TEST_SUITE
{
	TEST_RUN(null);
	TEST_RUN(ionull);
	TEST_RUN(iostd);
	TEST_RUN(ioerr);
	TEST_RUN(interact);
	CLEANUP_MSTR;
}
