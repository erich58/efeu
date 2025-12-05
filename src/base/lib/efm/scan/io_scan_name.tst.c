#include <EFEU/UnitTest.h>
#include <EFEU/ioscan.h>
#include <EFEU/utest/in.tst.h>

TEST_SUITE
{
	EXPECT_STR(io_scan_name(NULL, NULL), NULL);
	EXPECT_STR(io_scan_name(IN("123"), NULL), NULL);
	EXPECT_STR(io_scan_name(IN("a"), NULL), "a");

	IO *in = IN("a42.");
	EXPECT_STR(io_scan_name(in, NULL), "a42");
	EXPECT(io_getc(in) == '.');
	CLEANUP_IN;
}
