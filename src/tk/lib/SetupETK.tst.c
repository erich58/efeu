#include <EFEU/UnitTest.h>
#include <GUI/ETK.h>
#include "utest/eval.tst.h"

#if	HAS_TCL

TEST_DECL(tkinit)
{
	EXPECT_EVAL("tkinit()", "true");
	EXPECT_EVAL("tkinit()", "true");
}

TEST_DECL(tkmain)
{
	unsetenv("DISPLAY");
	EXPECT_EVAL("tkinit()", "false");
	PRINT_EVAL("tclget()");
	EXPECT_EVAL("tkevent()", "");
	EXPECT_EVAL("tkmain()", "");
}

TEST_DECL(tclset)
{
	EXPECT_EVAL("fmt_float = \"%#*.*f\"", "\"%#*.*f\"");
	EXPECT_EVAL("float_prec = 2", "2");

	EXPECT_EVAL("tclset(true); tclget()", "1");
	EXPECT_EVAL("tclset(42); tclget()", "42");
	EXPECT_EVAL("tclset(2.5); tclget()", "2.50");
	EXPECT_EVAL("tclset(\"abc\"); tclget()", "\"abc\"");

	EXPECT_EVAL("tclvar(\"a\", true); tcl::a", "1");
	EXPECT_EVAL("tclvar(\"b\", 42); tcl::b", "42");
	EXPECT_EVAL("tclvar(\"c\", 2.5); tcl::c", "2.50");
	EXPECT_EVAL("tclvar(\"d\", \"abc\"); tcl::d", "\"abc\"");

	EXPECT_EVAL("tclexpr(\"expr 3 * 5\")", "\"15\"");

	EXPECT_EVAL("tclfunc tst (int a, int b)  { tclset(a * b); }", "");
	EXPECT_EVAL("tclexpr(\"tst 2 7\")", "\"14\"");
	EXPECT_EVAL("tclexpr(\"tst $1 $2\", 3, 4)", "\"12\"");
	EXPECT_EVAL("tclexpr(\"tst $1 9\", 2)", "\"18\"");
}

TEST_SUITE
{
	SetupETK();
	TEST_EXEC(tkinit);
	TEST_EXEC(tkmain);
	TEST_RUN(tclset);
	CLEANUP_EVAL;
}

#else
TEST_SUITE
{
	PRINT_LABEL("Tcl/Tk not availabel");
}
#endif
