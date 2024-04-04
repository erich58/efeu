#include <EFEU/UnitTest.h>
#include <GUI/ETK.h>

#if	HAS_TCL

TEST_SUITE
{
	SetupETK();

	ETK *etk = ETK_create();
	EXPECT(ETK_result(NULL) == NULL);

	EXPECT(ETK_eval(etk, "var x 5") == 1);

	Tcl_SetResult(etk->interp, "abc", TCL_VOLATILE);
	EXPECT_STR(Obj2str(ETK_result(etk)), "abc");

	UnitTest_stderr("stderr");
	EXPECT(ETK_eval(etk, "bad_command") == 0);
	UnitTest_stderr(NULL);
	MATCH_CONTENT("stderr", "tcl:.*bad_command");
}

#else
TEST_SUITE
{
	PRINT_LABEL("Tcl/Tk not availabel");
}
#endif
