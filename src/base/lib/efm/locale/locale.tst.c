#include <EFEU/UnitTest.h>
#include <EFEU/locale.h>

static const char *llist()
{
	static char buf[64];
	snprintf(buf, sizeof buf, "%s %s %s", 
		Locale.scan->name, Locale.print->name, Locale.date->name);
	return buf;
}

TEST_SUITE
{
	EXPECT_STR(llist(), "C C C");
	PushLocale();

	SetLocale(LOC_SCAN, "de");
	SetLocale(LOC_PRINT, "TeX");
	EXPECT_STR(llist(), "de TeX C");
	PushLocale();

	SetLocale(LOC_DATE, "de");
	EXPECT_STR(llist(), "de TeX de");
	SetLocale(LOC_DATE, "de_AT");
	EXPECT_STR(llist(), "de TeX de_AT");
	SetLocale(LOC_DATE, "xx");
	EXPECT_STR(llist(), "de TeX C");

	PopLocale();
	EXPECT_STR(llist(), "de TeX C");
	PopLocale();
	EXPECT_STR(llist(), "C C C");

	SetLocale(LOC_SCAN|LOC_PRINT, "de");
	EXPECT_STR(llist(), "de de C");
	SetLocale(LOC_SCAN|LOC_PRINT, "TeX");
	EXPECT_STR(llist(), "TeX TeX C");
	SetLocale(LOC_SCAN|LOC_PRINT, "doc");
	EXPECT_STR(llist(), "doc doc C");
	SetLocale(LOC_SCAN|LOC_PRINT, "xx");
	EXPECT_STR(llist(), "C C C");
}
