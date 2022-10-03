#include <EFEU/UnitTest.h>
#include <EFEU/object.h>
#include <EfiType/SC.h>
#include "utest/term.tst.h"

TEST_DECL(create)
{
	EXPECT_TERM("SC(8,16)", "SC: 8x16", NULL);
}


TEST_SUITE
{
	SetupStd();
	SC_setup();
	TEST_RUN(create);
	CLEANUP_TERM;
}
