#include <EFEU/UnitTest.h>
#include <EFEU/ioctrl.h>
#include <EFEU/io.h>
#include <EFEU/LangDef.h>

/*	Testausgabe
*/

static char out_data[64];
static size_t out_lim = sizeof out_data - 1;
static size_t out_pos = 0;

static int out_put (int c, IO *io)
{
	if	(out_pos < out_lim)
		return (unsigned char) (out_data[out_pos++] = c);

	return EOF;
}

static int out_ctrl (IO *io, int req, va_list list)
{
	switch (req)
	{
	case IO_ERROR:	return 0;	// Kein exit(1) bei Fehler!
	default:	return EOF;
	}
}

static IO out_io = STD_IODATA (NULL, out_put, out_ctrl, "<tst>");
static IO *out = &out_io;

static void out_reset()
{
	out_lim = sizeof out_data - 1;
	out_pos = 0;
	out_io.stat = 0;
}

static void out_set(int lim)
{
	out_reset();

	if	(out_lim > lim)
		out_lim = lim;
}

static char *output()
{
	out_data[out_pos] = 0;
	out_reset();
	return out_data;
}

#define	EXPECT_OUTPUT(b) \
	EXPECT_STR(output(), b)

TEST_DECL(puts)
{
	EXPECT(io_putc('x', NULL) == 0);
	EXPECT(io_nputc('x', NULL, 2) == 0);
	EXPECT(io_puts("abc", NULL) == 0);
	EXPECT(io_nlputs("abc", NULL) == 0);

	out_set(1);
	EXPECT(io_putc('x', out) == 'x');
	EXPECT(io_putc('x', out) == EOF);
	EXPECT(out->stat == IO_STAT_ERR);

	out_set(2);
	EXPECT(io_nputc('*', out, 5) == 2);
	EXPECT(out->stat == IO_STAT_ERR);
	EXPECT_OUTPUT("**");

	out_set(2);
	EXPECT(io_puts("abc", out) == 2);
	EXPECT(out->stat == IO_STAT_ERR);
	EXPECT_OUTPUT("ab");

	out_reset();
	EXPECT(io_nlputs("abc\n", out) == 4);
	EXPECT(out->stat == IO_STAT_OK);
	EXPECT_OUTPUT("abc\n");
	EXPECT(io_nlputs("abc", out) == 4);
	EXPECT(out->stat == IO_STAT_OK);
	EXPECT_OUTPUT("abc\n");

	out_set(3);
	EXPECT(io_nlputs("abc", out) == 3);
	EXPECT(out->stat == IO_STAT_ERR);
	EXPECT_OUTPUT("abc");

	out_set(2);
	EXPECT(io_nlputs("abc", out) == 2);
	EXPECT(out->stat == IO_STAT_ERR);
	EXPECT_OUTPUT("ab");
}

TEST_DECL(xputs)
{
	EXPECT(io_xputc('x', NULL, "") == 0);
	EXPECT(io_xputs("abc", NULL, "") == 0);

	out_reset();
	EXPECT(io_xputs(NULL, out, "") == 0);
	EXPECT(io_xputs("\b\f\n\r\t\v\\", out, NULL) == 14);
	EXPECT(out->stat == IO_STAT_OK);
	EXPECT_OUTPUT("\\b\\f\\n\\r\\t\\v\\\\");
	EXPECT(io_xputs("abc'def'xyz;", out, "';") == 15);
	EXPECT(out->stat == IO_STAT_OK);
	EXPECT_OUTPUT("abc\\'def\\'xyz\\;");
	EXPECT(io_xputs("\001a", out, NULL) == 5);
	EXPECT(out->stat == IO_STAT_OK);
	EXPECT_OUTPUT("\\001a");

	out_set(2);
	EXPECT(io_xputs("a\bc", out, NULL) == 2);
	EXPECT(out->stat == IO_STAT_ERR);
	EXPECT_OUTPUT("a\\");
}

TEST_DECL(printf)
{
	va_list args;
	EXPECT(io_printf(NULL, "fmt") == 0);
	EXPECT(io_vprintf(NULL, "fmt", args) == 0);
	EXPECT(io_xprintf(NULL, "fmt") == 0);
	EXPECT(io_vxprintf(NULL, "fmt", args) == 0);

	out_reset();
	EXPECT(io_printf(out, "fmt") == 3);
	EXPECT_OUTPUT("fmt");
	EXPECT(io_xprintf(out, "fmt") == 3);
	EXPECT_OUTPUT("fmt");
	EXPECT(io_vprintf(out, "fmt", args) == 3);
	EXPECT_OUTPUT("fmt");
	EXPECT(io_vxprintf(out, "fmt", args) == 3);
	EXPECT_OUTPUT("fmt");
}

TEST_DECL(write)
{
	EXPECT(io_write(NULL, "1234", 4) == 0);
	EXPECT(io_rwrite(NULL, "1234", 4) == 0);

	out_reset();
	EXPECT(io_write(out, "1234", 4) == 4);
	EXPECT_OUTPUT("1234");
	EXPECT(io_rwrite(out, "1234", 4) == 4);
	EXPECT_OUTPUT("4321");

	out_set(2);
	EXPECT(io_write(out, "1234", 4) == 2);
	EXPECT(out->stat == IO_STAT_ERR);
	EXPECT_OUTPUT("12");
	out_set(2);
	EXPECT(io_rwrite(out, "1234", 4) == 2);
	EXPECT(out->stat == IO_STAT_ERR);
	EXPECT_OUTPUT("43");
}

TEST_DECL(langputs)
{
	SetLangDef("de_AT");
	out_reset();
	EXPECT(io_langputs(":de:ABC:*:XYZ", out) == 3);
	EXPECT_OUTPUT("ABC");
	out_set(2);
	EXPECT(io_langputs(":de:ABC:*:XYZ", out) == 2);
	EXPECT(out->stat == IO_STAT_ERR);
	EXPECT_OUTPUT("AB");
}

TEST_DECL(wputs)
{
	EXPECT(io_wputs("abc", NULL) == 0);

	out_set(2);
	EXPECT(io_wputs("abc", out) == 2);
	EXPECT(out->stat == IO_STAT_ERR);
	EXPECT_OUTPUT("ab");

	out_reset();
	io_ucswmode(out, "X");
	EXPECT(io_wputs("<a>", out) == 3);
	EXPECT_OUTPUT("&lt;a&gt;");
	io_ucswmode(out, "C");
	EXPECT(io_wputs("a\342\202\254", out) == 2);
	EXPECT_OUTPUT("a?");

	out_reset();
	EXPECT(io_putucs_ascii('a', out) == 1);
	EXPECT(io_putucs_ascii(0xFF, out) == 1);
	EXPECT(io_putucs_ascii(0x100, out) == 1);
	EXPECT_OUTPUT("a\xff?");

	EXPECT(io_putucs_latin1('a', out) == 1);
	EXPECT(io_putucs_latin1(0xFF, out) == 1);
	EXPECT(io_putucs_latin1(0x100, out) == 1);
	EXPECT_OUTPUT("a\xff?");

	EXPECT(io_putucs_latin9('a', out) == 1);
	EXPECT(io_putucs_latin9(0x20AC, out) == 1);
	EXPECT_OUTPUT("a\244");

	EXPECT(io_putucs_xml('a', out) == 1);
	EXPECT(io_putucs_xml('<', out) == 4);
	EXPECT(io_putucs_xml('>', out) == 4);
	EXPECT(io_putucs_xml('&', out) == 5);
	EXPECT(io_putucs_xml('\'', out) == 6);
	EXPECT(io_putucs_xml('"', out) == 6);
	EXPECT(io_putucs_xml(0x7f, out) == 6);
	EXPECT_OUTPUT("a&lt;&gt;&amp;&apos;&quot;&#127;");

//	EXPECT(io_putucs_xml(0x20AC, out) == 3);
//	EXPECT_OUTPUT("a\342\202\254");

	EXPECT(io_putucs_utf8(EOF, out) == 0);
	EXPECT(io_putucs_utf8('a', out) == 1);
	EXPECT(io_putucs_utf8(0x20AC, out) == 3);
	EXPECT_OUTPUT("a\342\202\254");

	EXPECT(io_putucs_utf8(0x7ff, out) == 2);
	EXPECT_OUTPUT("\xdf\xbf");
	EXPECT(io_putucs_utf8(0xffff, out) == 3);
	EXPECT_OUTPUT("\xef\xbf\xbf");
	EXPECT(io_putucs_utf8(0x1fffff, out) == 4);
	EXPECT_OUTPUT("\xf7\xbf\xbf\xbf");
	EXPECT(io_putucs_utf8(0x3000001, out) == 5);
	EXPECT_OUTPUT("\xfb\x80\x80\x80\x81");
	EXPECT(io_putucs_utf8(0x40000001, out) == 6);
	EXPECT_OUTPUT("\xfd\x80\x80\x80\x80\x81");
}

TEST_SUITE
{
	TEST_RUN(puts);
	TEST_RUN(xputs);
	TEST_RUN(printf);
	TEST_RUN(write);
	TEST_RUN(langputs);
	TEST_RUN(wputs);
}
