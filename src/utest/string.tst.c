#include <EFEU/UnitTest.h>
#include <stdlib.h>

static STRING_DECL(buf);
static UnitTestString *ustr = &buf;

TEST_DECL(cstr)
{
	EXPECT_STR(UnitTest_cstr(NULL), NULL);

	// typical use
	char *a = strdup("data");
	const char *b = UnitTest_cstr(a);
	free(a);
	EXPECT_STR(b, "data");
}

TEST_DECL(qstr)
{
	EXPECT_STR(UnitTest_qstr(0), "(null)");
	EXPECT_STR(UnitTest_qstr(":ä:€:"), "\":\\303\\244:\\342\\202\\254:\"");
	EXPECT_STR(UnitTest_qstr("a\tb"), "\"a\\tb\"");
	EXPECT_STR(UnitTest_qstr("a\nb"), "\"a\\nb\"");
	EXPECT_STR(UnitTest_qstr("a\fb"), "\"a\\fb\"");
	EXPECT_STR(UnitTest_qstr("a\bb"), "\"a\\bb\"");
	EXPECT_STR(UnitTest_qstr("a\vb"), "\"a\\vb\"");
	EXPECT_STR(UnitTest_qstr("a\rb"), "\"a\\rb\"");
	EXPECT_STR(UnitTest_qstr("a\"b"), "\"a\\\"b\"");
	EXPECT_STR(UnitTest_qstr("a\\b"), "\"a\\\\b\"");
}

TEST_DECL(string)
{
	UnitTest_puts("abc,", ustr);
	UnitTest_printf(ustr, "%d,", 42);
	EXPECT_STR(UnitTest_str(ustr), "abc,42,");
	UnitTest_qputs("x'y", '\'', ustr);
	EXPECT_STR(UnitTest_str(ustr), "abc,42," "'x\\'y'");
	UnitTest_clear(ustr);
	UnitTest_printf(ustr, "%d", 42);
	EXPECT_STR(UnitTest_str(ustr), "42");

	UnitTest_clear(ustr);
	UnitTest_puts("abc", ustr);
	UnitTest_printf(ustr, "%*s", 2000, "");
	UnitTest_puts("xyz", ustr);
	EXPECT(buf.size == 3 + 2000 + 3);
	buf.size = 4;
	EXPECT_STR(UnitTest_str(ustr), "abc ");

	UnitTest_free(ustr);
	EXPECT(buf.data == NULL);
	EXPECT(buf.size == 0);
	EXPECT(buf.capacity == 0);
	EXPECT(buf.pos == 0);
	TEST_EXPR(UnitTest_putc('a', NULL));
	TEST_EXPR(UnitTest_puts("abc", NULL));
	TEST_EXPR(UnitTest_printf(NULL, "%d", 42));
	EXPECT_STR(UnitTest_str(NULL), "");

	UnitTest_puts("abc", ustr);
	UnitTest_putc(0, ustr);
	UnitTest_putc(0, ustr);
	UnitTest_puts("xyz", ustr);
	EXPECT(UnitTest_start(ustr) == 8);
	EXPECT_STR(UnitTest_next(ustr), "abc");
	EXPECT_STR(UnitTest_next(ustr), "");
	EXPECT_STR(UnitTest_next(ustr), "xyz");
	EXPECT_STR(UnitTest_next(ustr), NULL);
}

TEST_DECL(split)
{
	EXPECT_STR(UnitTest_split(NULL, "abc xyz"), "abc xyz");
	EXPECT_STR(UnitTest_split(ustr, NULL), NULL);
	EXPECT_STR(UnitTest_split(ustr, "abc xyz"), "abc xyz");
	EXPECT_STR(UnitTest_next(ustr), NULL);
	EXPECT_STR(UnitTest_split(ustr, "\"abc xyz\""), "\"abc xyz\"");
	EXPECT_STR(UnitTest_next(ustr), NULL);

	EXPECT_STR(UnitTest_split(ustr, "\"abc\"  \"xyz\""), "");
	EXPECT_STR(UnitTest_next(ustr), "\"abc\"");
	EXPECT_STR(UnitTest_next(ustr), "\"xyz\"");
	EXPECT_STR(UnitTest_next(ustr), NULL);
}

TEST_DECL(qsplit)
{
	EXPECT_STR(UnitTest_qsplit(NULL, "abc\n"), "\"abc\\n\"");
	EXPECT_STR(UnitTest_qsplit(ustr, NULL), "(null)");
	EXPECT_STR(UnitTest_next(ustr), NULL);
	EXPECT_STR(UnitTest_qsplit(ustr, "abc\n"), "\"abc\\n\"");
	EXPECT_STR(UnitTest_next(ustr), NULL);

	EXPECT_STR(UnitTest_qsplit(ustr, "abc\n\nxyz\n"), "");
	EXPECT_STR(UnitTest_next(ustr), "\"abc\\n\"");
	EXPECT_STR(UnitTest_next(ustr), "\"\\n\"");
	EXPECT_STR(UnitTest_next(ustr), "\"xyz\\n\"");
	EXPECT_STR(UnitTest_next(ustr), NULL);
}

TEST_DECL(cleanup)
{
	EXPECT(ustr->data != NULL);
	EXPECT(ustr->capacity != 0);
	TEST_EXPR(UnitTest_free(ustr));
	EXPECT(ustr->data == NULL);
	EXPECT(ustr->capacity == 0);
}

TEST_SUITE
{
	TEST_RUN(cstr);
	TEST_RUN(qstr);
	TEST_RUN(string);
	TEST_RUN(split);
	TEST_RUN(qsplit);
	TEST_RUN(cleanup);
}
