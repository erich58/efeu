#include <EFEU/UnitTest.h>
#include <EFEU/io.h>
#include <EFEU/ioctrl.h>
#include <EFEU/MakeDepend.h>

#include "utest/mstr.tst.h"

TEST_DECL(diropen)
{
	IO *io;
	MakeDepend = 0;
	EXPECT(diropen(NULL, NULL) == NULL);
	TEST_FILE("not_a_dir", "");

	TEST_FORK
	{
		UnitTest_stderr("");
		diropen(UnitTest_tmp("not_a_dir"), "main.txt");
		exit(0);
	}

	CHILD_EXIT(1);

	EXPECT_PTR(io = diropen(UnitTest_tmp(NULL), "main.txt"));
	EXPECT(io_ctrl(io, 0) == EOF);
	EXPECT(io_rewind(io) == EOF);

	EXPECT(io_puts("Start\n", io));
	EXPECT(io_newpart(io, "chapter.txt", "include $1\n") == 0); 
	EXPECT(io_puts("Chapter\n", io));
	EXPECT(io_newpart(io, "section.txt", "include $1\n") == 0); 
	EXPECT(io_puts("Section\n", io));
	EXPECT(io_endpart(io) == 0); 
	EXPECT(io_puts("Data\n", io));
	EXPECT(io_endpart(io) == 0); 
	EXPECT(io_puts("End\n", io));
	EXPECT(io_close(io) == 0);

	EXPECT_CONTENT("main.txt",
		"Start\n"
		"include chapter.txt\n"
		"End\n"
	);
	EXPECT_CONTENT("chapter.txt",
		"Chapter\n"
		"include section.txt\n"
		"Data\n"
	);
	EXPECT_CONTENT("section.txt",
		"Section\n"
	);

	MakeDepend = 1;
	EXPECT_PTR(io = diropen(UnitTest_tmp(NULL), "test.txt"));
	io_puts("Test\n", io);
	EXPECT(io_close(io) == 0);
	EXPECT_CONTENT("test.txt", NULL);
}

TEST_DECL(push)
{
	IO *io;
	IO *sub;
	TEST_EXPR(io_push(NULL, NULL));
	EXPECT_PTR(io = io_string("abc", NULL));
	EXPECT(io_getc(io) == 'a');
	PRINT_MSTR(rd_ident(io));
	TEST_EXPR(io_push(io, io_string("xyz", NULL)));
	PRINT_MSTR(rd_ident(io));
	EXPECT(io_getc(io) == 'x');
	EXPECT(io_getc(io) == 'y');
	EXPECT(io_getc(io) == 'z');
	EXPECT(io_ungetc('z', io));
	PRINT_MSTR(rd_ident(io));

	EXPECT_PTR(sub = io_pop(io));
	PRINT_MSTR(rd_ident(sub));
	EXPECT(io_getc(sub) == 'z');
	EXPECT(io_close(sub) == 0);

	PRINT_MSTR(rd_ident(io));
	EXPECT(io_getc(io) == 'b');
	EXPECT(io_getc(io) == 'c');
	EXPECT(io_getc(io) == EOF);

	EXPECT(io_pop(io) == NULL);
	PRINT_MSTR(rd_ident(io));
	EXPECT(io_close(io) == 0);
}

TEST_DECL(lnum)
{
	IO *io;
	EXPECT_PTR(io = io_lnum(io_string("a\nb\n", NULL)));
	EXPECT_MSTR(rd_ident(io), "\"a\\nb\\n\":0:1");
	EXPECT(io_ctrl(io, IO_LINE) == 1);
	EXPECT(io_getc(io) == 'a');
	EXPECT(io_getc(io) == '\n');
	EXPECT(io_ctrl(io, IO_LINE) == 1);
	EXPECT(io_ctrl(io, IO_UNGETC, '\n'));
	EXPECT(io_getc(io) == '\n');
	EXPECT(io_ctrl(io, IO_LINE) == 1);
	EXPECT(io_getc(io) == 'b');
	EXPECT(io_ctrl(io, IO_LINE) == 2);
	EXPECT(io_rewind(io) == 0);
	EXPECT(io_ctrl(io, IO_LINE) == 1);
	EXPECT(io_ctrl(io, 0) == EOF);
	EXPECT(io_close(io) == 0);
}

TEST_DECL(lmark)
{
	IO *io;
	EXPECT_PTR(io = io_fopen(UnitTest_tmp("data"), "w"));
	EXPECT_PTR(io = io_lmark(io, "<", ">", 1));
	io_puts("line1\n\nline2\n", io);
	EXPECT(io_close(io) == 0);
	EXPECT_CONTENT("data", "<line1>\n<>\n<line2>\n");

	EXPECT_PTR(io = io_fopen(UnitTest_tmp("data"), "w+"));
	EXPECT_PTR(io = io_lmark(io, "%d: ", NULL, 0));
	MATCH_MSTR(rd_ident(io), "lmark(.*/data)");
	io_puts("line1\n\nline2\n", io);
	EXPECT(io_ctrl(io, 0) == EOF);
	EXPECT(io_rewind(io) == 0);
	EXPECT(io_close(io) == 0);
	EXPECT_CONTENT("data", "1: line1\n\n3: line2\n");
}

TEST_DECL(count)
{
	IO *io;
	EXPECT_PTR(io = io_count(rd_refer(ionull)));
	EXPECT(io_puts("abc", io) == 3); 
	EXPECT(io_ctrl(io, 0) == EOF);
	EXPECT(io_ctrl(io, IO_REWIND) == 3);
	EXPECT(io_puts("1234", io) == 4); 
	EXPECT(io_puts("567", io) == 3); 
	EXPECT(io_close(io) == 7);

	EXPECT_PTR(io = io_count(NULL));
	EXPECT(io_puts("abc", io) == 3); 
	EXPECT(io_ctrl(io, IO_REWIND) == EOF);
	EXPECT(io_close(io) == 3);
}

TEST_DECL(indent)
{
	IO *io;
	EXPECT_PTR(io = io_fopen(UnitTest_tmp("data"), "w"));
	EXPECT_PTR(io = io_indent(io, '=', 2));
	MATCH_MSTR(rd_ident(io), "indent(.*/data)");
	EXPECT(io_ctrl(io, 0) == EOF);
	io_puts("line1\n",io);
	io_submode(io, 1);
	io_puts("line2\n",io);
	io_submode(io, 1);
	io_puts("line3\n",io);
	io_submode(io, 0);
	io_puts("line4\n",io);
	io_submode(io, 0);
	io_puts("line5\n",io);
	EXPECT(io_rewind(io) == 0);
	EXPECT(io_close(io) == 0);
	EXPECT_CONTENT("data",
		"line1\n"
		"==line2\n"
		"====line3\n"
		"==line4\n"
		"line5\n"
	);
}

TEST_DECL(divide)
{
	IO *base;
	IO *sub;
	EXPECT_PTR(base = io_fopen(UnitTest_tmp("data"), "w"));
	EXPECT_PTR(sub = io_divide(base, 0));
	MATCH_MSTR(rd_ident(sub), "divide(.*/data)");
	EXPECT(io_ctrl(sub, 0) == EOF);
	io_puts("b1", base);
	io_puts("s1", sub);
	io_puts("b2", base);
	io_puts("s2", sub);
	io_ctrl(sub, IO_FLUSH);
	io_puts("b3", base);
	io_puts("s3", sub);
	io_close(sub);
	io_close(base);
	EXPECT_CONTENT("data", "b1b2s1s2b3s3");
}

TEST_DECL(crlf)
{
	IO *io;
	EXPECT_PTR(io = io_fopen(UnitTest_tmp("data"), "w"));
	EXPECT_PTR(io = io_crlf(io));
	EXPECT(io_ctrl(io, 0) == EOF);
	io_puts("abc\n\nxyz\n",io);
	EXPECT(io_rewind(io) == 0);
	EXPECT(io_close(io) == 0);
	EXPECT_CONTENT("data", "abc\r\n\r\nxyz\r\n");
}

TEST_DECL(latin9)
{
	IO *io;
	EXPECT(out_latin9(NULL) == NULL);
	EXPECT_PTR(io = io_fopen(UnitTest_tmp("data"), "w"));
	EXPECT_PTR(io = out_latin9(io));
	MATCH_MSTR(rd_ident(io), "latin9(.*/data)");
	EXPECT(io_ctrl(io, 0) == EOF);

	io_puts("|ä€|", io);
	io_putc(EOF, io);
	io_puts("\xef\xbf\xbf|", io);
	io_puts("\xf0\x9a\x80\x8b|", io);
	io_puts("\xf0", io);
	io_ctrl(io, IO_FLUSH);
	io_putucs(0x20ac, io);

	EXPECT(io_rewind(io) == 0);
	EXPECT(io_close(io) == 0);
	EXPECT_CONTENT("data", "|\344\244|?|?|\360\244");
}

TEST_DECL(utf8)
{
	IO *io;
	EXPECT(out_utf8(NULL) == NULL);
	EXPECT_PTR(io = io_fopen(UnitTest_tmp("data"), "w"));
	EXPECT_PTR(io = out_utf8(io));
	MATCH_MSTR(rd_ident(io), "utf8(.*/data)");
	EXPECT(io_ctrl(io, 0) == EOF);

	io_puts("|ä€|", io);
	io_putc(EOF, io);
	io_puts("\xef\xbf\xbf|", io);
	io_puts("\xf0\x9a\x80\x8b|", io);
	io_puts("\xf0", io);
	io_ctrl(io, IO_FLUSH);
	io_putucs(0x20ac, io);
	io_putc('|', io);
	io_putc(255, io);

	EXPECT(io_rewind(io) == 0);
	EXPECT(io_close(io) == 0);

	EXPECT_CONTENT("data", "|\303\244\342\202\254|\357\277\277|"
		"\360\232\200\213|\303\260\342\202\254|\303\277");
}

static int protect_line(IO *io, int c, void *par)
{
	return (c == '#');
}

TEST_DECL(lang)
{
	IO *io;
	char buf[32];
	TEST_EXPR(langfilter(NULL, NULL)) == NULL;

	TEST_FILE("data",
		":en:\tabc\n"
		":*:\t\\\txy:=z\n"
		"\n"
		":en,*:\\abc\n"
		"\n"
		"123:de:abc:*:xyz:_:456\n"
		"123:de:abc:*:xyz:_:456\n"
		":en:456\n"
		"#:de_*,de_AT:A\\:C:*:X\\:Z\n"
		"\n"
		"a:de_DE:23:de_*:45:*:67:_:\n"
		"b:*:67:de_*:45:de_DE:23:_:\n"
		"a|:de:abc:*:xyz|\n"
	);

	EXPECT_PTR(io = io_fopen(UnitTest_tmp("data"), "r"));
	EXPECT_PTR(io = xlangfilter(io, "de_AT", protect_line, NULL));
	MATCH_MSTR(rd_ident(io), "[de_AT].*");

	EXPECT_STR(io_gets(buf, sizeof buf, io), "\txy:=z\n");
	EXPECT_STR(io_gets(buf, sizeof buf, io), "\n");
	EXPECT_STR(io_gets(buf, sizeof buf, io), "\\abc\n");
	EXPECT_STR(io_gets(buf, sizeof buf, io), "\n");
	io_protect(io, 1);
	EXPECT_STR(io_gets(buf, sizeof buf, io), "123:de:abc:*:xyz:_:456\n");
	io_protect(io, 0);
	EXPECT_STR(io_gets(buf, sizeof buf, io), "123abc456\n");
	EXPECT_STR(io_gets(buf, sizeof buf, io), "#A:C\n");
	EXPECT_STR(io_gets(buf, sizeof buf, io), "a45\n");
	EXPECT_STR(io_gets(buf, sizeof buf, io), "b45\n");
	EXPECT_STR(io_gets(buf, sizeof buf, io), "a|:de:abc:*:xyz|\n");
	EXPECT_STR(io_gets(buf, sizeof buf, io), NULL);

	EXPECT(io_rewind(io) == EOF);
	io_close(io);
}

TEST_SUITE
{
	TEST_RUN(diropen);
	TEST_RUN(push);
	TEST_RUN(lnum);
	TEST_RUN(lmark);
	TEST_RUN(count);
	TEST_RUN(indent);
	TEST_RUN(divide);
	TEST_RUN(crlf);
	TEST_RUN(latin9);
	TEST_RUN(utf8);
	TEST_RUN(lang);
	CLEANUP_MSTR;
}
