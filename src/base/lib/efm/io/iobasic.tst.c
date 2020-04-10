#include <EFEU/UnitTest.h>
#include <EFEU/ioctrl.h>
#include <EFEU/mstring.h>
#include <sys/types.h>
#include <sys/stat.h>

TEST_DECL(string)
{
	IO *io;
	char buf[32];
	unsigned offset;

	EXPECT((io = io_string("data", NULL)) != NULL);
	EXPECT_STR(io_gets(buf, sizeof buf, io), "data");
	EXPECT_STR(rd_ident(io), "\"data\":4");
	EXPECT(io_rewind(io) == 0);
	offset = 2;
	EXPECT(io_ctrl(io, IO_SETPOS, &offset) == 0);
	EXPECT(io_getc(io) == 't');
	EXPECT(io_ctrl(io, IO_GETPOS, &offset) == 0);
	EXPECT(offset == 3);
	offset = 5;
	EXPECT(io_ctrl(io, IO_SETPOS, &offset) == EOF);
	EXPECT(io_close(io) == 0);

	EXPECT((io = io_string(msprintf("%-100d", 42), memfree)) != NULL);
	EXPECT(io_getc(io) == '4');
	EXPECT(io_getc(io) == '2');
	MATCH_STR(rd_ident(io), "\"42[ ]*[.]{3}\":2");
	EXPECT(io_close(io) == 0);

	EXPECT((io = io_string(NULL, NULL)) == NULL);
}

TEST_DECL(stream)
{
	IO *io;
	struct stat st;
	unsigned offset;
	char buf[32];
	int wd[2] = { 0x31323334, 0x61626364 };
	int rd[2] = { 0, 0 };

	EXPECT((io = io_fopen(UnitTest_tmpname("data"), "w+")) != NULL);
	EXPECT_STR(rd_ident(io), UnitTest_tmpname("data"));
	EXPECT(io_putc('T', io) == 'T');
	EXPECT(io_dbwrite(io, wd, sizeof wd[0], 2) == sizeof wd);
	EXPECT(io_rewind(io) == 0);
	EXPECT(io_peek(io) == 'T');
	EXPECT(io_getc(io) == 'T');
	EXPECT(io_dbread(io, rd, sizeof rd[0], 2) == sizeof rd);
	EXPECT(memcmp(wd, rd, sizeof wd) == 0);

	EXPECT(io_ctrl(io, IO_GETPOS, &offset) == 0);
	EXPECT(offset == 9);
	EXPECT(io_ctrl(io, IO_SETPOS, &offset) == 0);
	EXPECT(io_ctrl(io, 0) == EOF);
	EXPECT(io_ctrl(io, IO_FDIN) > 0);
	EXPECT(io_ctrl(io, IO_FDOUT) > 0);
	EXPECT(io_ctrl(io, IO_FLUSH) == 0);
	EXPECT(io_ctrl(io, IO_STAT, &st) == 0);
	EXPECT(st.st_size == 9);
	EXPECT(io_close(io) == 0);

	EXPECT_CONTENT("data", "T1234abcd");

	EXPECT((io = io_tmpfile()) != NULL);
	EXPECT_STR(rd_ident(io), "<tmpfile>");
	EXPECT(io_puts("data\n", io) == 5);
	EXPECT(io_rewind(io) == 0);
	EXPECT_STR(io_gets(buf, sizeof buf, io), "data\n");
	EXPECT(io_close(io) == 0);

	EXPECT((io = io_fileopen("|echo test", "r")) != NULL);
	EXPECT_STR(rd_ident(io), "<!echo>");
	EXPECT(io_ctrl(io, IO_GETPOS, &offset) == EOF);
	EXPECT_STR(io_gets(buf, sizeof buf, io), "test\n");
	EXPECT(io_close(io) == 0);

	EXPECT((io = io_fileopen(NULL, "w")) != NULL);
	EXPECT_STR(rd_ident(io), "<stdout>");
	EXPECT(io_close(io) == 0);

	EXPECT((io = io_findopen(UnitTest_tmpname(NULL),
		"data", NULL, "r")) != NULL);
	EXPECT(io_close(io) == 0);
}

TEST_DECL(strbuf)
{
	char buf[32];
	unsigned offset;
	IO *io;

	StrBuf *sb = sb_create(32);
	EXPECT((io = io_strbuf(sb)) != NULL);
	EXPECT_STR(rd_ident(io), "<strbuf>");
	EXPECT(io_puts("data", io) == 4);
	EXPECT(io_rewind(io) == 0);
	EXPECT_STR(io_gets(buf, sizeof buf, io), "data");
	EXPECT(io_rewind(io) == 0);
	offset = 2;
	EXPECT(io_ctrl(io, IO_SETPOS, &offset) == 0);
	EXPECT(io_getc(io) == 't');
	EXPECT(io_ctrl(io, IO_GETPOS, &offset) == 0);
	EXPECT(offset == 3);
	EXPECT(io_ctrl(io, IO_UNGETC, 'x') == 'x');
	EXPECT(io_getc(io) == 'x');
	EXPECT(io_ctrl(io, 0) == EOF);
	EXPECT(io_close(io) == 0);
	sb_destroy(sb);

	EXPECT((io = io_tmpbuf(64)) != NULL);
	EXPECT_STR(rd_ident(io), "<tmpbuf>");
	EXPECT(io_puts("data", io) == 4);
	EXPECT(io_rewind(io) == 0);
	EXPECT_STR(io_gets(buf, sizeof buf, io), "data");
	EXPECT(io_close(io) == 0);

	EXPECT((io = io_strbuf(NULL)) != NULL);
	EXPECT_STR(rd_ident(io), "<tmpbuf>");
	EXPECT(io_close(io) == 0);
}

TEST_DECL(data)
{
	IO *io;
	char dbuf[16] = "T1234abcd";
	unsigned offset;
	int wd[2] = { 0x31323334, 0x61626364 };
	int rd[2] = { 0, 0 };

	EXPECT((io = io_data(NULL, dbuf, sizeof dbuf)) != NULL);
	EXPECT(io_peek(io) == 'T');
	EXPECT(io_getc(io) == 'T');
	EXPECT(io_dbread(io, rd, sizeof rd[0], 2) == sizeof rd);
	EXPECT(memcmp(wd, rd, sizeof wd) == 0);
	EXPECT_STR(rd_ident(io), ":9/16");
	EXPECT(io_rewind(io) == 0);
	EXPECT(io_ctrl(io, IO_GETPOS, &offset) == 0);
	EXPECT(offset == 0);
	TEST_EXPR(offset = 10);
	EXPECT(io_ctrl(io, IO_SETPOS, &offset) == 0);
	EXPECT(io_getc(io) == 0);
	TEST_EXPR(offset = 16);
	EXPECT(io_ctrl(io, IO_SETPOS, &offset) == EOF);

	TEST_FORK
	{
		UnitTest_stderr("");
		io_dbread(io, rd, sizeof rd[0], 2);
		exit(0);
	}

	CHILD_EXIT(1);

	EXPECT(io_close(io) == 0);

	EXPECT(io_data(NULL, NULL, 0) == NULL);
}

TEST_DECL(bigbuf)
{
	IO *io;
	char buf[32];

	EXPECT((io = io_bigbuf(4, UnitTest_tmpname("buf_"))) != NULL);
	PRINT_STR(rd_ident(io));
	EXPECT(io_puts("abcd1234567", io) == 11);
	MATCH_STR(rd_ident(io), "bigbuf[(].*[)] 2[+]3 2[+]0");
	EXPECT(io_rewind(io) == 0);
	EXPECT(io_read(io, buf, sizeof buf) == 11);
	buf[11] = 0;
	EXPECT_STR(buf, "abcd1234567");
	EXPECT(io_close(io) == 0);
}

TEST_DECL(copy)
{
	char *str = "abc123xyz";
	IO *in;
	IO *out;
	EXPECT((in = io_string(str, NULL)) != NULL);
	EXPECT((out = io_fopen(UnitTest_tmpname("data"), "w")) != NULL);
	EXPECT(io_copy(in, out) == strlen(str));
	EXPECT(io_close(in) == 0);
	EXPECT(io_close(out) == 0);
	EXPECT_CONTENT("data", str);
}

TEST_SUITE
{
	TEST_RUN(string);
	TEST_RUN(stream);
	TEST_RUN(strbuf);
	TEST_RUN(data);
	TEST_RUN(bigbuf);
	TEST_RUN(copy);
}
