#include <EFEU/UnitTest.h>

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
	STRING_DECL(buf);

	UnitTest_puts("abc,", &buf);
	UnitTest_printf(&buf, "%d,", 42);
	EXPECT_STR(UnitTest_str(&buf), "abc,42,");
	UnitTest_qputs("x'y", '\'', &buf);
	EXPECT_STR(UnitTest_str(&buf), "abc,42," "'x\\'y'");
	UnitTest_clear(&buf);
	UnitTest_printf(&buf, "%d", 42);
	EXPECT_STR(UnitTest_str(&buf), "42");

	UnitTest_clear(&buf);
	UnitTest_puts("abc", &buf);
	UnitTest_printf(&buf, "%*s", 2000, "");
	UnitTest_puts("xyz", &buf);
	EXPECT(buf.size == 3 + 2000 + 3);
	buf.size = 4;
	EXPECT_STR(UnitTest_str(&buf), "abc ");

	UnitTest_free(&buf);
	EXPECT(buf.data == NULL);
	EXPECT(buf.size == 0);
	EXPECT(buf.capacity == 0);
	EXPECT(buf.pos == 0);
	TEST_EXPR(UnitTest_putc('a', NULL));
	TEST_EXPR(UnitTest_puts("abc", NULL));
	TEST_EXPR(UnitTest_printf(NULL, "%d", 42));
	EXPECT_STR(UnitTest_str(NULL), "");

	UnitTest_puts("abc", &buf);
	UnitTest_putc(0, &buf);
	UnitTest_putc(0, &buf);
	UnitTest_puts("xyz", &buf);
	EXPECT(UnitTest_start(&buf) == 8);
	EXPECT_STR(UnitTest_next(&buf), "abc");
	EXPECT_STR(UnitTest_next(&buf), "");
	EXPECT_STR(UnitTest_next(&buf), "xyz");
	EXPECT_STR(UnitTest_next(&buf), NULL);

	EXPECT_STR(UnitTest_qsplit(NULL, "abc\n"), "\"abc\\n\"");
	EXPECT_STR(UnitTest_qsplit(&buf, NULL), "(null)");
	EXPECT_STR(UnitTest_next(&buf), NULL);
	EXPECT_STR(UnitTest_qsplit(&buf, "abc\n"), "\"abc\\n\"");
	EXPECT_STR(UnitTest_next(&buf), NULL);

	EXPECT_STR(UnitTest_qsplit(&buf, "abc\n\nxyz\n"), "");
	EXPECT_STR(UnitTest_next(&buf), "\"abc\\n\"");
	EXPECT_STR(UnitTest_next(&buf), "\"\\n\"");
	EXPECT_STR(UnitTest_next(&buf), "\"xyz\\n\"");
	EXPECT_STR(UnitTest_next(&buf), NULL);

	UnitTest_free(&buf);
}

TEST_SUITE
{
	TEST_RUN(cstr);
	TEST_RUN(qstr);
	TEST_RUN(string);
}
