#include <EFEU/UnitTest.h>
#include <EFEU/ioscan.h>
#include <EFEU/utest/in.tst.h>

TEST_DECL(not_a_char)
{
	EXPECT_STR(io_scan_char(NULL, NULL), NULL);
	EXPECT_STR(io_scan_char(IN("a"), NULL), NULL);
}

TEST_DECL(std)
{
	IO *in = IN("'a'x");
	EXPECT_STR(io_scan_char(in, NULL), "a");
	EXPECT(io_getc(in) == 'x');

	EXPECT_STR(io_scan_char(IN("'ä'"), NULL), "\303\244");
	EXPECT_STR(io_scan_char(IN("'\\ä'"), NULL), "\303\244");
	EXPECT_STR(io_scan_char(IN("'€'"), NULL), "\342\202\254");
	EXPECT_STR(io_scan_char(IN("'\\''"), NULL), "'");
	EXPECT_STR(io_scan_char(IN("'\\n'"), NULL), "\n");
}

TEST_DECL(incomplete)
{
	EXPECT_STR(io_scan_char(IN("'"), NULL), "");
	EXPECT_STR(io_scan_char(IN("''"), NULL), "");
	EXPECT_STR(io_scan_char(IN("'\\"), NULL), "\\");
	EXPECT_STR(io_scan_char(IN("'\\'"), NULL), "'");
}

TEST_DECL(extra)
{
	UnitTest_stderr("err");
	EXPECT_STR(io_scan_char(IN("'ab'"), NULL), "a");
	UnitTest_stderr(NULL);
	EXPECT_CONTENT("err",
		"extra character b in character constant ignored.\n"
	);
	UnitTest_stderr("err");
	EXPECT_STR(io_scan_char(IN("'ä€'"), NULL), "ä");
	UnitTest_stderr(NULL);
	EXPECT_CONTENT("err",
		"extra character € in character constant ignored.\n"
	);
}

TEST_SUITE
{
	TEST_RUN(not_a_char);
	TEST_RUN(std);
	TEST_RUN(incomplete);
	TEST_RUN(extra);
	CLEANUP_IN;
}
