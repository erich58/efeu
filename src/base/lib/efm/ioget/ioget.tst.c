#include <EFEU/UnitTest.h>
#include <EFEU/io.h>

#include "utest/in.tst.h"
#include "utest/mstr.tst.h"

#define	EXPECT_BUF(a, b) \
	UnitTest_strcheck(__FILE__, __LINE__, "==", sb_nul(a), b, # a, # b)

TEST_DECL(getarg)
{
	EXPECT_MSTR(io_getarg(IN("[a(b)c]"), '[', ']'), "a(b)c");
	EXPECT_MSTR(io_getarg(IN("[a(bc]"), '[', ']'), "a(bc]");
	EXPECT_MSTR(io_getarg(IN("[a(b{c)d]"), '[', ']'), "a(b{c)d]");
	EXPECT_MSTR(io_getarg(IN("|a[bc|d]|ef"), '|', '|'), "a[bc|d]");
	EXPECT_MSTR(io_getarg(IN("x|a"), '|', '|'), NULL);
}

TEST_DECL(gets)
{
	char buf[5];
	EXPECT_STR(io_gets(buf, sizeof buf, IN("123456")), "1234");
	EXPECT_STR(io_gets(buf, sizeof buf, IN("abc")), "abc");
	EXPECT_STR(io_gets(buf, sizeof buf, NULL), NULL);
	EXPECT_STR(io_gets(buf, sizeof buf, IN("")), NULL);
}


TEST_DECL(getc)
{
	StrBuf *sb;
	IO *io;
	int i;
	
	EXPECT(io_peek(NULL) == EOF);
	EXPECT(io_getc(NULL) == EOF);
	EXPECT(io_ungetc('a', NULL) == 0);

	EXPECT(io_peek(ionull) == EOF);
	EXPECT(io_getc(ionull) == EOF);
	EXPECT(io_ungetc('a', ionull));
	EXPECT(io_peek(ionull) == 'a');
	EXPECT(io_getc(ionull) == 'a');

	EXPECT_PTR(io = IN("abc"));
	EXPECT(io_peek(io) == 'a');
	EXPECT(io_getc(io) == 'a');
	EXPECT(io_ungetc('x', io));
	EXPECT(io_peek(io) == 'x');
	EXPECT(io_getc(io) == 'x');

	sb = sb_create(32);
	EXPECT(sb_puts("1", sb) == 1);
	sb_setpos(sb, 0);
	io = io_strbuf(sb);
	EXPECT(io_peek(io) == '1');
	EXPECT(io_getc(io) == '1');
	EXPECT(io_peek(io) == EOF);
	EXPECT(io_getc(io) == EOF);

	for (i = 0; i < IO_MAX_SAVE; i++)
		EXPECT(io_ungetc('x', io));

	EXPECT(io_ungetc('a', io));
	EXPECT(sb->data[0] == 'x');
	EXPECT(io_ungetc('a', io) == 0);

	sb_clean(sb);
	EXPECT(sb_puts("1", sb) == 1);
	io_rewind(io);
	io_push(io, io_string("x", NULL));
	EXPECT(io_getc(io) == 'x');
	EXPECT(io_getc(io) == '1');
	EXPECT(io_getc(io) == EOF);

	io_rewind(io);
	io_push(io, io_string("x", NULL));
	EXPECT(io_getc(io) == 'x');
	EXPECT(io_peek(io) == '1');

	sb_destroy(sb);
}

TEST_DECL(mgetc)
{
	IO *in;
	EXPECT_PTR(in = IN("\\a\\\n \tb"));
	EXPECT(io_mgetc(in, 0) == '\\');
	EXPECT(io_mgetc(in, 0) == 'a');
	EXPECT(io_mgetc(in, 0) == ' ');

	EXPECT_PTR(in = IN("\\a\\\n \t\\\n b"));
	EXPECT(io_mgetc(in, 1) == '\\');
	EXPECT(io_mgetc(in, 1) == 'a');
	EXPECT(io_mgetc(in, 1) == 'b');

	EXPECT(io_ungetc('\n', in));
	EXPECT(io_ungetc('\\', in));
	EXPECT(io_mgetc(in, 0) == '\n');

	EXPECT(io_mgetc(NULL, 0) == EOF);

	EXPECT_MSTR(io_mgets(IN("abc\\\nxyz\\"), NULL), "abcxyz\\");
	EXPECT_MSTR(io_mgets(IN("abc\\;x\\yz;"), ";"), "abc;x\\yz");
}

TEST_DECL(xgetc)
{
	IO *in;
	EXPECT_MSTR(io_xgets(IN("\\b\\f\\n\\r\\t\\v\\a\\c\\"), NULL),
		"\b\f\n\r\t\v\007\\c");
	EXPECT_MSTR(io_xgets(IN(":\\xa0:\\xfg:\\xa"), NULL),
		":\240:\017g:\012");
	EXPECT_MSTR(io_xgets(IN(":\\240:\\248:\\20"), NULL),
		":\240:\0248:\020");

	EXPECT_PTR(in = IN("a\\\\b\\\nc\\;d\\ef;xyz"));
	EXPECT_MSTR(io_xgets(in, ";"), "a\\bc;d\\ef");
	EXPECT(io_getc(in) == ';');
	EXPECT_MSTR(io_xgets(in, ";"), "xyz");
}

TEST_DECL(read)
{
	char buf[64];

	strcpy(buf, "xxxxx");
	EXPECT(io_read(IN("123456"), buf, 4) == 4);
	EXPECT_STR(buf, "1234x");
	strcpy(buf, "xxxxx");
	EXPECT(io_read(IN("12"), buf, 4) == 2);
	EXPECT_STR(buf, "12xxx");

	strcpy(buf, "xxxxx");
	EXPECT(io_rread(IN("123456"), buf, 4) == 4);
	EXPECT_STR(buf, "4321x");
	strcpy(buf, "xxxxx");
	EXPECT(io_rread(IN("12"), buf, 4) == 2);
	EXPECT_STR(buf, "xx21x");

	EXPECT_MSTR(io_mread(IN("123456"), 4), "1234");
	EXPECT_MSTR(io_mread(IN("12"), 4), "12");
}

TEST_DECL(getline)
{
	StrBuf *sb = sb_create(64);
	EXPECT_STR(io_getline(IN("abc\ndef\n"), sb, EOF), "abc\ndef\n");
	EXPECT_STR(io_getline(IN("abc\ndef\n"), sb, '\n'), "abc");

	EXPECT_STR(io_mgetline(IN("abc\ndef\n"), sb, NULL), "abc\ndef\n");
	EXPECT_STR(io_mgetline(IN("abc\ndef\n"), sb, "\n"), "abc");
	EXPECT_STR(io_mgetline(IN("a\\;\\bc\\\nd;ef\n"), sb, ";\n"), "a;\\bcd");
	EXPECT_STR(io_mgetline(IN("abc\\"), sb, ";\n"), "abc\\");

	sb_destroy(sb);

	EXPECT_MSTR(io_getline(IN(""), NULL, '\n'), NULL);
	EXPECT_MSTR(io_getline(IN("abc\ndef\n"), NULL, '\n'), "abc");
	EXPECT_MSTR(io_mgetline(IN("abc\\\ndef\n"), NULL, "\n"), "abcdef");
}

TEST_DECL(getucs)
{
	EXPECT(io_getucs_latin1(IN("\244")) == 0244);
	EXPECT(io_getucs_latin9(IN("\244")) == 0x20AC);
	EXPECT(io_getucs_utf8(IN("\177")) == 0177);
	EXPECT(io_getucs_utf8(IN("\342\202\254")) == 0x20AC);
	EXPECT(io_getucs_utf8(IN("\xc2\x80")) == 0x80);
	EXPECT(io_getucs_utf8(IN("\xdf\xbf")) == 0x7ff);
	EXPECT(io_getucs_utf8(IN("\xef\xbf\xbf")) == 0xffff);
	EXPECT(io_getucs_utf8(IN("\xf7\xbf\xbf\xbf")) == 0x1fffff);
	EXPECT(io_getucs_utf8(IN("\xfb\x80\x80\x80\x81")) == 0x3000001);
	EXPECT(io_getucs_utf8(IN("\xfd\x80\x80\x80\x80\x81")) == 0x40000001);
	EXPECT(io_getucs_utf8(IN("\xf0\x9a\x80\x8b")) == 0x1a00b);
	EXPECT(io_getucs_utf8(IN("\xfe")) == EOF);
	EXPECT(io_getucs_utf8(IN("")) == EOF);

	EXPECT(io_getucs(IN("\x7f")) == 0x7f);
	EXPECT(io_getucs(IN("\xdf\xbf")) == 0x7ff);
	EXPECT(io_getucs(IN("\xef\xbf\xbf")) == 0xffff);
	EXPECT(io_getucs(IN("\xf7\xbf\xbf\xbf")) == 0x1fffff);
	EXPECT(io_getucs(IN("\xf7.")) == 0xf7);
	EXPECT(io_getucs(IN("\xf7")) == 0xf7);
	EXPECT(io_getucs(IN("\244.")) == 0x20AC);
	EXPECT(io_getucs(IN("")) == EOF);
	EXPECT(io_getucs(IN("x")) == 'x');

	EXPECT(io_ungetucs('x', NULL) == EOF);
	EXPECT(io_ungetucs(0x20AC, ionull) == 0x20AC);
	EXPECT(io_getucs(ionull) == 0x20AC);
	EXPECT(io_getucs(NULL) == EOF);
}

TEST_SUITE
{
	TEST_RUN(getarg);
	TEST_RUN(getc);
	TEST_RUN(gets);
	TEST_RUN(mgetc);
	TEST_RUN(xgetc);
	TEST_RUN(read);
	TEST_RUN(getline);
	TEST_RUN(getucs);

	// Speicherfreigabe
	IN(NULL);
	utest_mstr(NULL);
}
