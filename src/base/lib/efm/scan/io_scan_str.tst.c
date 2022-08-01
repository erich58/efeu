#include <EFEU/UnitTest.h>
#include <EFEU/ioscan.h>
#include "utest/in.tst.h"


TEST_SUITE
{
	EXPECT_STR(io_scan_str(NULL, NULL), NULL);
	EXPECT_STR(io_scan_str(IN("a"), NULL), NULL);
	IO *in = IN("\"abc\"x");
	EXPECT_STR(io_scan_str(in, NULL), "abc");
	EXPECT(io_getc(in) == 'x');
	CLEANUP_IN;
}
