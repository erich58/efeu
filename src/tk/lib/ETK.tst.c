#include <EFEU/UnitTest.h>
#include <GUI/ETK.h>

#if	HAS_TCL

#include "utest/mstr.tst.h"

TEST_SUITE
{
	SetupStd();
	ETK *etk = ETK_create();
	EXPECT(etk != NULL);
	EXPECT_MSTR(rd_ident(etk), "Tcl/Tk interface");
	EXPECT_INT(Obj2int(strterm("int x")), 0);
	EXPECT(ETK_eval(etk, "@eval x = 42"));
	EXPECT_INT(Obj2int(strterm("x")), 42);
	EXPECT(ETK_eval(etk, "@expr x"));
	rd_deref(etk);
	CLEANUP_MSTR;
}

#else
TEST_SUITE
{
	PRINT_LABEL("Tcl/Tk not availabel");
}
#endif
