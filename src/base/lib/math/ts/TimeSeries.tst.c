#include <EFEU/UnitTest.h>
#include <EFEU/object.h>
#include <Math/TimeSeries.h>
#include "utest/eval.tst.h"

TEST_DECL(setup)
{
	SetupStd();
	SetupMath();
	SetupTimeSeries();
}

TEST_SUITE
{
	TEST_RUN(setup);
	CLEANUP_EVAL;
}
