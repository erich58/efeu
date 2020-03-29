#include <EFEU/UnitTest.h>
#include <EFEU/MakeDepend.h>
#include "utest/out.tst.h"

TEST_SUITE
{
	AddTarget("t1");
	AddTarget("t2");
	AddDepend("d1");
	AddDepend("d2");
	MakeDependRule(OUT());
	EXPECT_STR(DATA(), "\nt1 t2: d1 d2\n");
	MakeTargetRule(OUT(), "command $< $@");
	EXPECT_STR(DATA(), "\nt1 t2: d1 d2\n\tcommand $< $@\n");
	MakeTaskRule(OUT(), "task-name");
	EXPECT_STR(DATA(), "\ntask-name:: t1 t2\n");
	MakeCleanRule(OUT(), "clean-name");
	EXPECT_STR(DATA(), "\nclean-name::\n\trm -f t1 t2\n");
}
