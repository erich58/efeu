#include <EFEU/UnitTest.h>
#include <EFEU/io.h>
#include <EFEU/ioctrl.h>
#include <EFEU/procenv.h>

/* Fehlerprüfung
*/

static int echeck_put (int c, IO *io)
{
	return EOF;
}

static int echeck_ctrl (IO *io, int req, va_list list)
{
	switch (req)
	{
	case IO_ERROR:	return 0;	// Kein exit(1) bei Fehler!
	default:	return EOF;
	}
}

static IO echeck = STD_IODATA (NULL, echeck_put, echeck_ctrl, "<echeck>");

TEST_DECL(setup)
{
	SetApplPath(UnitTest_tmp(NULL));
	TEST_FILE("ftools.msg",
		"21	ERROR 21.\n"
		"22	ERROR 22.\n"
	);

}

TEST_DECL(binval)
{
	IO *io;
	ASSERT_PTR(io = io_tmpbuf(0));

	EXPECT(io_putval(0x40414243u, io, 4) == 4);
	EXPECT(io_putstr(NULL, io) == 1);
	EXPECT(io_putstr("abc", io) == 5);
	io_rewind(io);

	EXPECT(io_getval(io, 4) == 0x40414243u);
	EXPECT_STR(io_getstr(io), NULL);
	EXPECT_STR(io_getstr(io), "abc");
	io_rewind(io);

	EXPECT(io_getc(io) == 0x40);
	EXPECT(io_getc(io) == 0x41);
	EXPECT(io_getc(io) == 0x42);
	EXPECT(io_getc(io) == 0x43);
	EXPECT(io_getc(io) == 0);
	EXPECT(io_getc(io) == 1);
	EXPECT(io_getc(io) == 3);
	EXPECT(io_getc(io) == 'a');
	EXPECT(io_getc(io) == 'b');
	EXPECT(io_getc(io) == 'c');
	io_close(io);

	echeck.stat = IO_STAT_OK;
	EXPECT(io_putstr(NULL, &echeck) == 0);
	EXPECT(io_putstr("abc", &echeck) == 0);
	EXPECT_STR(io_getstr(&echeck), NULL);

	UnitTest_stderr("stderr");
	echeck.stat = IO_STAT_OK;
	EXPECT(io_getval(&echeck, 4) == 0);
	EXPECT_CONTENT("stderr", "ERR ERROR 21.\n");

	UnitTest_stderr("stderr");
	EXPECT(io_putval(1u, &echeck, 4) == 0);
	EXPECT_CONTENT("stderr", "ERR ERROR 22.\n");
}

TEST_DECL(llget)
{
	IO *io;
	ASSERT_PTR(io = io_tmpbuf(0));

	EXPECT(io_llput(0x404142434445ll, io, 6) == 6);
	EXPECT(io_llput(-128ll, io, 1) == 1);
	EXPECT(io_llput(-1ll, io, 1) == 1);
	EXPECT(io_llput(-1ll, io, 2) == 2);
	EXPECT(io_llput(-1ll, io, 4) == 4);
	EXPECT(io_llput(-1ll, io, 8) == 8);
	io_rewind(io);

	EXPECT(io_llget(io, 6) == 0x404142434445ll);
	EXPECT(io_llget(io, 1) == -128ll);
	EXPECT(io_llget(io, 1) == -1ll);
	EXPECT(io_llget(io, 2) == -1ll);
	EXPECT(io_llget(io, 4) == -1ll);
	EXPECT(io_llget(io, 8) == -1ll);
	io_rewind(io);

	EXPECT(io_getc(io) == 0x40);
	EXPECT(io_getc(io) == 0x41);
	EXPECT(io_getc(io) == 0x42);
	EXPECT(io_getc(io) == 0x43);
	EXPECT(io_getc(io) == 0x44);
	EXPECT(io_getc(io) == 0x45);
	EXPECT(io_getc(io) == 0x80);
	io_close(io);

	UnitTest_stderr("stderr");
	echeck.stat = IO_STAT_OK;
	EXPECT(io_llget(&echeck, 4) == 0);
	EXPECT_CONTENT("stderr", "ERR ERROR 21.\n");

	UnitTest_stderr("stderr");
	echeck.stat = IO_STAT_OK;
	EXPECT(io_llput(1u, &echeck, 4) == 0);
	EXPECT_CONTENT("stderr", "ERR ERROR 22.\n");
}

TEST_DECL(ullget)
{
	IO *io;
	ASSERT_PTR(io = io_tmpbuf(0));

	EXPECT(io_ullput(0x404142434445ull, io, 6) == 6);
	EXPECT(io_ullput(~0ull, io, 1) == 1);
	EXPECT(io_ullput(~0ull, io, 2) == 2);
	EXPECT(io_ullput(~0ull, io, 4) == 4);
	EXPECT(io_ullput(~0ull, io, 8) == 8);
	io_rewind(io);

	EXPECT(io_ullget(io, 6) == 0x404142434445ull);
	EXPECT(io_ullget(io, 1) == 0xFFull);
	EXPECT(io_ullget(io, 2) == 0xFFFFull);
	EXPECT(io_ullget(io, 4) == 0xFFFFFFFFull);
	EXPECT(io_ullget(io, 8) == 0xFFFFFFFFFFFFFFFFull);
	io_rewind(io);

	EXPECT(io_getc(io) == 0x40);
	EXPECT(io_getc(io) == 0x41);
	EXPECT(io_getc(io) == 0x42);
	EXPECT(io_getc(io) == 0x43);
	EXPECT(io_getc(io) == 0x44);
	EXPECT(io_getc(io) == 0x45);
	io_close(io);

	UnitTest_stderr("stderr");
	echeck.stat = IO_STAT_OK;
	EXPECT(io_ullget(&echeck, 4) == 0);
	EXPECT_CONTENT("stderr", "ERR ERROR 21.\n");

	UnitTest_stderr("stderr");
	echeck.stat = IO_STAT_OK;
	EXPECT(io_ullput(1u, &echeck, 4) == 0);
	EXPECT_CONTENT("stderr", "ERR ERROR 22.\n");
}

TEST_DECL(llread)
{
	IO *io;
	int64_t a[6] = { 0, 0x8000000000000000ll, 1, -1, 63, -63 };
	int64_t b[4] = { 64, -64, 8191, -8191 };
	int64_t c[6] = { 0 };

	ASSERT_PTR(io = io_tmpbuf(0));

	EXPECT_UINT(io_llwrite(io, a, 6), 6);
	EXPECT_UINT(io_llwrite(io, b, 4), 8);
	io_rewind(io);

	EXPECT(io_llread(io, c, 6) == 6);
	EXPECT(memcmp(a, c, sizeof a) == 0);
	EXPECT(io_llread(io, c, 4) == 8);
	EXPECT(memcmp(b, c, sizeof b) == 0);
	io_rewind(io);

	EXPECT(io_getc(io) == 0x0);
	EXPECT(io_getc(io) == 0x40);
	EXPECT(io_getc(io) == 0x1);
	EXPECT(io_getc(io) == 0x41);
	EXPECT(io_getc(io) == 0x3f);
	EXPECT(io_getc(io) == 0x7f);

	EXPECT(io_getc(io) == 0x80);
	EXPECT(io_getc(io) == 0x01);
	EXPECT(io_getc(io) == 0xc0);
	EXPECT(io_getc(io) == 0x01);
	EXPECT(io_getc(io) == 0xbf);
	EXPECT(io_getc(io) == 0x7f);
	EXPECT(io_getc(io) == 0xff);
	EXPECT(io_getc(io) == 0x7f);
	io_close(io);

	UnitTest_stderr("stderr");
	echeck.stat = IO_STAT_OK;
	EXPECT(io_llread(&echeck, c, 1) == 0);
	EXPECT_CONTENT("stderr", "ERR ERROR 21.\n");

	/*
	UnitTest_stderr("stderr");
	EXPECT(io_llwrite(&echeck, a, 1) == 0);
	EXPECT_CONTENT("stderr", "ERR ERROR 22.\n");
	*/
}

TEST_DECL(ullread)
{
	IO *io;
	uint64_t a[8] = {
		0, 0x7f,		// 2 * 1 Byte
		0x80, 0x81, 0x3fff,	// 3 * 2 Byte
		0x4000, 0x100000,	// 2 * 3 Byte
		~0,			// 1 * 9 Byte
	};

	uint64_t b[8] = { 0 };

	ASSERT_PTR(io = io_tmpbuf(0));

	EXPECT(io_ullwrite(io, a, 8) == 23);
	io_rewind(io);
	EXPECT(io_ullread(io, b, 8) == 23);
	EXPECT(memcmp(a, b, sizeof a) == 0);
	io_rewind(io);
	EXPECT(io_getc(io) == 0x0);
	EXPECT(io_getc(io) == 0x7f);
	EXPECT(io_getc(io) == 0x80);
	EXPECT(io_getc(io) == 0x01);
	io_close(io);

	UnitTest_stderr("stderr");
	echeck.stat = IO_STAT_OK;
	EXPECT(io_ullread(&echeck, b, 1) == 0);
	EXPECT_CONTENT("stderr", "ERR ERROR 21.\n");

	EXPECT(io_ullwrite(NULL, a, 1) == 0);
	/*
	UnitTest_stderr("stderr");
	EXPECT(io_ullwrite(&echeck, a, 1) == 0);
	EXPECT_CONTENT("stderr", "ERR ERROR 22.\n");
	*/
}

TEST_DECL(size)
{
	IO *io;
	size_t s;

	ASSERT_PTR(io = io_tmpbuf(0));

	EXPECT(io_write_size(0, io) == 1);
	EXPECT(io_write_size(0x80, io) == 2);
	EXPECT(io_write_size(0x4000, io) == 3);
	io_rewind(io);

	EXPECT(io_read_size(&s, io) == 1);
	EXPECT(s == 0);
	EXPECT(io_read_size(&s, io) == 2);
	EXPECT(s == 0x80);
	EXPECT(io_read_size(&s, io) == 3);
	EXPECT(s == 0x4000);
	io_rewind(io);

	EXPECT(io_read_size(&s, &echeck) == 0);
	EXPECT(s == 0);
}

TEST_DECL(iodata)
{
	IO *io;
	int wd1[3] = { 0, 12345, ~0 };
	int rd1[3] = { 0, 0, 0 };

	ASSERT_PTR(io = io_tmpbuf(0));
	EXPECT(io_dbwrite(io, wd1, sizeof wd1[0], 3) == 3 * sizeof wd1[0]);
	io_rewind(io);

	EXPECT(io_dbread(io, rd1, sizeof rd1[0], 3) == 3 * sizeof wd1[0]);
	EXPECT(memcmp(wd1, rd1, sizeof wd1) == 0);
	io_close(io);

	UnitTest_stderr("stderr");
	echeck.stat = IO_STAT_OK;
	EXPECT(io_dbread(&echeck, rd1, sizeof rd1[0], 3) == 0);
	EXPECT_CONTENT("stderr", "ERR ERROR 21.\n");

	UnitTest_stderr("stderr");
	echeck.stat = IO_STAT_ERR;
	EXPECT(io_dbread(&echeck, rd1, sizeof rd1[0], 3) == 0);
	EXPECT_CONTENT("stderr", "ERR ERROR 21.\n");

	UnitTest_stderr("stderr");
	echeck.stat = IO_STAT_ERR;
	EXPECT(io_dbwrite(&echeck, wd1, sizeof wd1[0], 3) == 0);
	EXPECT_CONTENT("stderr", "ERR ERROR 22.\n");
}

TEST_SUITE
{
	TEST_RUN(setup);
	TEST_RUN(binval);
	TEST_RUN(llget);
	TEST_RUN(ullget);
	TEST_RUN(llread);
	TEST_RUN(ullread);
	TEST_RUN(size);
	TEST_RUN(iodata);
}
