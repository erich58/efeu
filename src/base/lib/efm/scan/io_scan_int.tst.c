#include <EFEU/UnitTest.h>
#include <EFEU/ioscan.h>
#include "utest/in.tst.h"

#define	SCAN(input, val, rem)	do { \
	UnitTest_info(__FILE__, __LINE__, "INFO", \
		"io_scan_int(%s)", # input); \
	IO *in = IN(input); 	\
	const char *value = io_scan_int(in, NULL); \
	EXPECT_STR(value, val); \
	char remaining[8]; \
	io_gets(remaining, sizeof remaining, in); \
	EXPECT_STR(remaining, rem); \
} while (0)


TEST_DECL(not_a_int)
{
	SCAN(NULL, NULL, "");
	SCAN("a", NULL, "a");
	SCAN("+a", NULL, "+a");
	SCAN("-", NULL, "-");
}

TEST_DECL(std)
{
	SCAN("0u", "0", "u");
	SCAN("-342.", "-342", ".");
	SCAN("+342.", "+342", ".");
	SCAN("0378", "037", "8");
	SCAN("0xabcz", "0xabc", "z");
	SCAN("0XaBc ", "0XaBc", " ");
#if	IO_SCAN_BINARY
	SCAN("-0b12", "-0x1", "2");
	SCAN("+0B1111011100110001", "+0xf731", "");
#endif
}

TEST_SUITE
{
	TEST_RUN(not_a_int);
	TEST_RUN(std);
	CLEANUP_IN;
}
