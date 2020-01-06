#include <EFEU/UnitTest.h>

TEST_SUITE
{
	EXPECT_STREQ(UnitTest_qstr(0), "(null)");
	EXPECT_STREQ(UnitTest_qstr("äöü"), "\"äöü\"");
	EXPECT_STREQ(UnitTest_qstr("a\tb"), "\"a\\tb\"");
	EXPECT_STREQ(UnitTest_qstr("a\nb"), "\"a\\nb\"");
	EXPECT_STREQ(UnitTest_qstr("a\"b"), "\"a\\\"b\"");
}
