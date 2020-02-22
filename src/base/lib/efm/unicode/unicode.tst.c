#include <EFEU/UnitTest.h>
#include <EFEU/unicode.h>
#include "utest/data.tst.h"

#define	DATA_UTF8 "-€ŠšŽžŒœŸ-"
#define	DATA_LATIN9 "-\xa4\xa6\xa8\xb4\xb8\xbc\xbd\xbe-"

static int32_t getucs(char *def, int len)
{
	return pgetucs(&def, len);
}

static const char *utfdump(int32_t ucs)
{
	static unsigned char buf[8];
	int n = nputucs(ucs, buf, 7);
	int i;
	IO *io = out();
	io_printf(io, "%d", n);

	for (i = 0; i < n; i++)
		io_printf(io, " %2x", buf[i]);

	return data();
}

TEST_SUITE
{
	char buf[64];

	EXPECT_TRUE(ucscopy_utf8(NULL, 0, NULL) == 0);
	EXPECT_TRUE(ucscopy_utf8(buf, 64, NULL) == 0);
	EXPECT_TRUE(ucscopy_utf8(NULL, 0, "abc") == 3);
	EXPECT_TRUE(ucscopy_utf8(NULL, 0, "abc") == 3);
	EXPECT_TRUE(ucscopy_latin9(NULL, 0, NULL) == 0);
	EXPECT_TRUE(ucscopy_latin9(buf, 64, NULL) == 0);
	EXPECT_TRUE(ucscopy_latin9(NULL, 0, "abc") == 3);

	EXPECT_TRUE(ucscopy_utf8(buf, 3, "abc") == 3);
	EXPECT_STR(buf, "ab");

	EXPECT_TRUE(ucscopy_latin9(buf, 3, "xyz") == 3);
	EXPECT_STR(buf, "xy");

	ucscopy_utf8(buf, 64, DATA_LATIN9);
	EXPECT_STR(buf, DATA_UTF8);
	ucscopy_latin9(buf, 64, buf);
	EXPECT_STR(buf, DATA_LATIN9);

	EXPECT_TRUE(latin9_to_ucs(EOF) == EOF);
	EXPECT_TRUE(nputucs(-1, (unsigned char *) buf, 64) == 0);

	EXPECT_STR(utfdump(0x7f), "1 7f");
	EXPECT_STR(utfdump(0x80), "2 c2 80");
	EXPECT_STR(utfdump(0x7ff), "2 df bf");
	EXPECT_STR(utfdump(0x800), "3 e0 a0 80");
	EXPECT_STR(utfdump(0xffff), "3 ef bf bf");
	EXPECT_STR(utfdump(0x10000), "4 f0 90 80 80");
	EXPECT_STR(utfdump(0x1fffff), "4 f7 bf bf bf");
	EXPECT_STR(utfdump(0x200000), "0");

	EXPECT_TRUE(getucs("\xc2\x80", 1) == 0xc2);
	EXPECT_TRUE(getucs("\xc2\x80", 2) == 0x80);
	EXPECT_TRUE(getucs("\xc2\xf0", 2) == 0xc2);
	EXPECT_TRUE(getucs("\xef\xbf\xbf", 3) == 0xffff);
	EXPECT_TRUE(getucs("\xf0\x9a\x80\x8b", 4) == 0x1a00b);
}
