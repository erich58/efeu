#include <EFEU/UnitTest.h>

TEST_SUITE
{
	STRING_DECL(buf);

	UnitTest_puts("abc,", &buf);
	UnitTest_printf(&buf, "%d,", 42);
	EXPECT_STR(UnitTest_cstr(&buf), "abc,42,");
	UnitTest_qputs("x'y", '\'', &buf);
	EXPECT_STR(UnitTest_cstr(&buf), "abc,42,'x\\'y'");
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
	TEST_EXPR(UnitTest_putc('a', NULL));
	TEST_EXPR(UnitTest_puts("abc", NULL));
	TEST_EXPR(UnitTest_printf(NULL, "%d", 42));
	EXPECT_STR(UnitTest_cstr(NULL), "");
}
