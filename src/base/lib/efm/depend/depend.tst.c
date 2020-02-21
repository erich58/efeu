#include <EFEU/UnitTest.h>
#include <EFEU/MakeDepend.h>
#include "utest/data.tst.h"

TEST_SUITE
{
	AddTarget("t1");
	AddTarget("t2");
	AddDepend("d1");
	AddDepend("d2");
	MakeDependRule(out());
	EXPECT_STR(data(), "\nt1 t2: d1 d2\n");
	MakeTargetRule(out(), "command $< $@");
	EXPECT_STR(data(), "\nt1 t2: d1 d2\n\tcommand $< $@\n");
	MakeTaskRule(out(), "task-name");
	EXPECT_STR(data(), "\ntask-name:: t1 t2\n");
	MakeCleanRule(out(), "clean-name");
	EXPECT_STR(data(), "\nclean-name::\n\trm -f t1 t2\n");
}
