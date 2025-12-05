#include <EFEU/UnitTest.h>
#include <EFEU/MakeDepend.h>
#include <EFEU/utest/out.tst.h>

TEST_SUITE
{
	AddTarget("target_file_1");
	AddTarget("target_file_2");
	AddDepend("depend_file_1");
	AddDepend("depend_file_2");
	AddDepend("depend_file_3");
	AddDepend("depend_file_4");
	MakeDependList(OUT(), "target_name_list");
	EXPECT_OUT("\n"
"target_name_list: depend_file_1 depend_file_2 depend_file_3 \\\n"
" depend_file_4\n"
	);
	MakeDependRule(OUT());
	EXPECT_OUT("\n"
"target_file_1 target_file_2: depend_file_1 depend_file_2 \\\n"
" depend_file_3 depend_file_4\n"
	);
	MakeTargetRule(OUT(), "command $< $@");
	EXPECT_OUT("\n"
"target_file_1 target_file_2: depend_file_1 depend_file_2 \\\n"
" depend_file_3 depend_file_4\n"
"\tcommand $< $@\n"
	);
	MakeTaskRule(OUT(), "task-name");
	EXPECT_OUT("\ntask-name:: target_file_1 target_file_2\n");
	MakeCleanRule(OUT(), "clean-name");
	EXPECT_OUT("\nclean-name::\n\trm -f target_file_1 target_file_2\n");
	CLEANUP_OUT;
}
