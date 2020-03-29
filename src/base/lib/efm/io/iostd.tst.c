#include <EFEU/UnitTest.h>
#include <EFEU/ioctrl.h>

TEST_DECL(null)
{
	EXPECT_STR(rd_ident(NULL), NULL);
	EXPECT(io_getc(NULL) == EOF);
	EXPECT(io_puts("test\n", NULL) == 0);

	EXPECT(io_ctrl(NULL, IO_CLOSE) == EOF);
}

TEST_DECL(ionull)
{
	EXPECT_STR(rd_ident(ionull), "<null>");
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
	EXPECT_STR(rd_ident(iostd), "<stdin,stdout>");

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
	EXPECT_STR(rd_ident(ioerr), "<stderr>");

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

TEST_SUITE
{
	TEST_RUN(null);
	TEST_RUN(ionull);
	TEST_RUN(iostd);
	TEST_RUN(ioerr);
}
