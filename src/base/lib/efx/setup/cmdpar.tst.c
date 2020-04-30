#include <EFEU/UnitTest.h>
#include <EFEU/object.h>
#include <EFEU/Resource.h>
#include <EFEU/math.h>
#include <EFEU/CmdPar.h>
//#include "utest/mstr.tst.h"
#include "utest/in.tst.h"
#include "utest/out.tst.h"
//#include "utest/obj.tst.h"
#include <math.h>

TEST_DECL(cmdpar)
{
	static CmdPar cpar;

	CmdPar_expand(&cpar, IN("proto \rtypelist"), OUT());
	EXPECT_OUT("|List_t typelist ()|\n");
	CmdPar_expand(&cpar, IN("proto \rType_t::info "), OUT());
	EXPECT_OUT("|void Type_t::info (str mode = NULL)|\n");
}

TEST_SUITE
{
	SetupStd();
	SetupMath();
	TEST_RUN(cmdpar);
//	CLEANUP_MSTR;
	CLEANUP_IN;
	CLEANUP_OUT;
//	CLEANUP_OBJ;
}
