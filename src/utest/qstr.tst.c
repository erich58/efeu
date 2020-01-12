#include <EFEU/UnitTest.h>

TEST_SUITE
{
	EXPECT_STREQ(UnitTest_qstr(0), "(null)");
	EXPECT_STREQ(UnitTest_qstr("äöü"), "\"äöü\"");
	EXPECT_STREQ(UnitTest_qstr("a\tb"), "\"a\\tb\"");
	EXPECT_STREQ(UnitTest_qstr("a\nb"), "\"a\\nb\"");
	EXPECT_STREQ(UnitTest_qstr("a\fb"), "\"a\\fb\"");
	EXPECT_STREQ(UnitTest_qstr("a\bb"), "\"a\\bb\"");
	EXPECT_STREQ(UnitTest_qstr("a\vb"), "\"a\\vb\"");
	EXPECT_STREQ(UnitTest_qstr("a\rb"), "\"a\\rb\"");
	EXPECT_STREQ(UnitTest_qstr("a\"b"), "\"a\\\"b\"");
	EXPECT_STREQ(UnitTest_qstr("a\\b"), "\"a\\\\b\"");
}
