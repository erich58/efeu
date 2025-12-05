#include <EFEU/UnitTest.h>
#include <EFEU/ioscan.h>
#include <EFEU/utest/in.tst.h>

#define	IO_SCAN_STR(in)	io_scan_str(in, NULL)

#define	SCAN_REM(input, val, rem)	\
	EXPECT_IO2STR(IO_SCAN_STR, input, val, rem)

#define	SCAN(input, val)	SCAN_REM(input, val, NULL)


TEST_SUITE
{
	SCAN(NULL, NULL);
	SCAN("\t", NULL);
	SCAN_REM("a3", NULL, "a3");
	SCAN_REM("\"abc\" x", "abc", " x");
	SCAN_REM("\"\" x", "", " x");
	SCAN_REM("\"\"\" x", "", "\" x");

	CLEANUP_IN;
}
