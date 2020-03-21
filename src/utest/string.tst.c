#include <EFEU/UnitTest.h>

TEST_SUITE
{
	STRING_DECL(buf);

	UnitTest_puts("abc,", &buf);
	UnitTest_printf(&buf, "%d,", 42);
	EXPECT_STR(UnitTest_cstr(&buf), "abc,42,");
	UnitTest_qputs("x'y", '\'', &buf);
	EXPECT_STR(UnitTest_cstr(&buf), "abc,42," "'x\\'y'");
	UnitTest_clear(&buf);
	UnitTest_printf(&buf, "%d", 42);
	EXPECT_STR(UnitTest_cstr(&buf), "42");

	UnitTest_clear(&buf);
	UnitTest_puts("abc", &buf);
	UnitTest_printf(&buf, "%*s", 2000, "");
	UnitTest_puts("xyz", &buf);
	EXPECT(buf.size == 3 + 2000 + 3);
	buf.size = 4;
	EXPECT_STR(UnitTest_cstr(&buf), "abc ");

	UnitTest_free(&buf);
	EXPECT(buf.data == NULL);
	EXPECT(buf.size == 0);
	EXPECT(buf.capacity == 0);
	EXPECT(buf.pos == 0);
	TEST_EXPR(UnitTest_putc('a', NULL));
	TEST_EXPR(UnitTest_puts("abc", NULL));
	TEST_EXPR(UnitTest_printf(NULL, "%d", 42));
	EXPECT_STR(UnitTest_cstr(NULL), "");

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
