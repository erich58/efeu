#include <EFEU/UnitTest.h>
#include <GUI/ETK.h>
//#include "utest/eval.tst.h"

#if	HAS_TCL

TEST_SUITE
{
	SetupETK();
}

#else
TEST_SUITE
{
	PRINT_LABEL("Tcl/Tk not availabel");
}
#endif
