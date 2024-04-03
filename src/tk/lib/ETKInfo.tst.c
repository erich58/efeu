#include <EFEU/UnitTest.h>
#include <GUI/ETK.h>

extern char *IncPath;

TEST_SUITE
{
	TEST_FILE("EisTk.hdr",
		"void EisTk(str name)\n"
		"{\n"
		"\tfprintf(ioerr, \"EisTk: %s\\n\", name);\n"
		"}\n"
	);
	IncPath = mstrcpy(UnitTest_tmp(NULL));
	UnitTest_stderr("stderr");
	ETKInfo("InfoNode");
	UnitTest_stderr(NULL);
	EXPECT_CONTENT("stderr", "EisTk: InfoNode\n");
}
