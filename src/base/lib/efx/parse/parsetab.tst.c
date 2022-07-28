#include <EFEU/UnitTest.h>
#include <EFEU/parsedef.h>
#include <EFEU/printobj.h>
#include "utest/out.tst.h"

TEST_SUITE
{
	EfiParseDef p0[] = {
		{ "test", PFunc_str, "Test-String" },
	};

	EfiParseDef p1[] = {
		{ "k2", PFunc_str, "Key 2" },
		{ "k1", PFunc_str, "Key 1" },
	};

	AddParseDef(p0, 1);
	PushParseTab(NULL);
	AddParseDef(p1, 2);
	ListParseDef(OUT());
	EXPECT_OUT(
		"Keywords of level 0:\n"
		"\tk1 k2\n"
		"Keywords of level 1:\n"
		"\ttest\n"
	);

	EXPECT(GetParseDef("test") == p0);
	EXPECT(GetParseDef("k1") == p1 + 1);
	DelParseTab(PopParseTab());
	DelParseTab(PopParseTab());
	CLEANUP_OUT;
}
