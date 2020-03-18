#include <EFEU/UnitTest.h>

TEST_SUITE
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
