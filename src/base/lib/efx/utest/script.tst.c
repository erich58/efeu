/*	Test-Framework für Skripts
*/

#include <EFEU/UnitTest.h>
#include <EFEU/pconfig.h>
#include <EFEU/preproc.h>
#include <EFEU/efutil.h>
#include <EFEU/printobj.h>

#define runUnitTest hidden_runUnitTest
void runUnitTest(int UnitTest_argc, char **UnitTest_argv);
#include "utest/UnitTest.ext.c"
#undef	runUnitTest

TEST_SUITE
{
	SetupStd();
	SetupMath();
	SetupUtil();
	SetupPreproc();
	SetupDebug();

	UnitTest_setup();

	if	(UnitTest_argc > 1)
	{
		UnitTest_name = UnitTest_argv[1];
		EshEval(&UnitTest_argc, UnitTest_argv);
	}
	else
	{
		EXPECT(EshEval(0, NULL) != 0);
		TEST_FILE("stdin",
			"EXPECT(6 * 7 == 42);\n"
		);
		UnitTest_stdin("stdin", NULL);
		EshEval(&UnitTest_argc, UnitTest_argv);
	}
}
