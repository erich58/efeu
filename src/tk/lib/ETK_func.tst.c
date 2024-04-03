#include <EFEU/UnitTest.h>
#include <GUI/ETK.h>
#include "utest/eval.tst.h"

#if	HAS_TCL

TEST_SUITE
{
	SetupETK();
	UnitTest_stderr("stderr");
	EXPECT_EVAL("tclfunc", "");
	UnitTest_stderr(NULL);
	EXPECT_CONTENT("stderr", "missing name\n");
	EXPECT_EVAL("tclfunc t1 (int a)  { tclset(a); }", "");
	EXPECT_EVAL("tclfunc \"t2\" (int a)  tclset(a);", "");
	CLEANUP_EVAL;
}

#else
TEST_SUITE
{
	PRINT_LABEL("Tcl/Tk not availabel");
}
#endif
