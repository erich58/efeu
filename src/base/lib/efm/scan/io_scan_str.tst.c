#include <EFEU/UnitTest.h>
#include <EFEU/ioscan.h>
#include "utest/in.tst.h"

#define	SCAN_REM(input, val, rem)	\
	IO2STR_FUNCTION(io_scan_str, (in, NULL), input, val, rem)

#define	SCAN(input, val)	SCAN_REM(input, val, NULL)


TEST_SUITE
{
	SCAN(NULL, NULL);
	SCAN("\t", NULL);
	SCAN_REM("a3", NULL, "a3");
	SCAN_REM("\"abc\" x", "abc", " x");
	SCAN_REM("\"\" x", "", " x");

	CLEANUP_IN;
}
