#include <EFEU/UnitTest.h>
#include <GUI/ETK.h>
#include "utest/eval.tst.h"
#include "utest/mstr.tst.h"

#if	HAS_TCL

TEST_SUITE
{
	SetupETK();

	ETK *etk = ETK_create();

	EXPECT_EVAL("fmt_float = \"%#*.*f\"", "\"%#*.*f\"");
	EXPECT_EVAL("float_prec = 2", "2");

	EXPECT_EVAL("tclvar(\"n\", 42); tcl::n", "42");
	EXPECT_EVAL("tclvar(\"d\", 2.5); tcl::d", "2.50");
	EXPECT_EVAL("tclvar(\"s\", \"abc\"); tcl::s", "\"abc\"");

	EXPECT_EVAL("tcl::n = 10", "10");
	EXPECT_EVAL("tcl::d = 1.", "1.00");
	EXPECT_EVAL("tcl::s = \"xyz\"", "\"xyz\"");

	EXPECT(ETK_var(NULL, NULL) == NULL);

	EfiObj *obj = ETK_var(etk, "n");
	EXPECT(obj->type == &Type_int);
	MATCH_MSTR(Obj_ident(obj), "int &tcl \\(tcl::n) \\{}");
	rd_deref(obj);

	CLEANUP_EVAL;
	CLEANUP_MSTR;
}

#else
TEST_SUITE
{
	PRINT_LABEL("Tcl/Tk not availabel");
}
#endif
