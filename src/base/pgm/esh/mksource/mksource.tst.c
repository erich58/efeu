#include <EFEU/UnitTest.h>
#include <stdlib.h>

#define	MS_SRC	SRC "/mksource"		// Source directory
#define	MS_BIN	BUILD "/mksource"	// Binary version with coverage
#define	MS_INC	" -I" MS_SRC "/files" 	// Include Files
#define	MS_NAME	MS_SRC "/mksource"	// Runtime name (defines ApplPath)

TEST_DECL(setup)
{
	setenv("LANG", "C", 1);
	PRINT_LABEL("Force separating UnitTest output from cmd output");
	UnitTest_stderr("stderr");
	UnitTest_stderr(NULL);

	TEST_FILE("test.ms",
		"void to_hdr(str txt) { fprintf(hdr, \"%s\\n\", txt); }\n"
		"void to_src(str txt) { fprintf(src, \"%s\\n\", txt); }\n"
	);
	TEST_FILE("simple.tpl",
		"#include <test.ms>\n"
		"//Title\n"
		"rule SimpleRule(tg,dep,cmd)\n"
		"config src tex\n"
		"/*\n"
		"Comment\n"
		"*/\n"
		"fprintf(src, \"COMMENT: %s\\n\", usecomment());\n"
		"to_hdr(\"Header\");\n"
		"to_src(\"Source\");\n"
		"addcomment(\"END\n\")\n"
	);
}

TEST_DECL(synopsis)
{
	TEST_FORK
	{
		setenv("TERM", "dumb", 1);
		setenv("COLUMNS", "60", 1);
		UnitTest_reopen(2, "stderr");
		CMD_EXEC(MS_BIN, MS_NAME " -?");
	}

	CHILD_EXIT(0);
	LMATCH_CONTENT("stderr",
		"SYNOPSIS\n"
		"mksource\n"
	);
}

#define	RUN_MKSOURCE(args)	\
TEST_FORK { \
	chdir(UnitTest_tmp(NULL)); \
	UnitTest_reopen(1, "stdout"); \
	UnitTest_reopen(2, "stderr"); \
	CMD_EXEC(MS_BIN, MS_NAME " -I. " args); } \
CHILD_EXIT(0);

TEST_DECL(rules)
{
	RUN_MKSOURCE("./simple -r");
	LMATCH_CONTENT("stderr",
		"mksource -I. ./simple -r\n"
		"input < ./simple.tpl\n"
	);
	LMATCH_CONTENT("stdout",
		"\n"
		"# Rules created with\n"
		"# .*/mksource -I. ./simple -r\n"
		"all:: simple.c simple.tex\n"
		"depend:: ./simple.tpl\n"
		"	.*/mksource -I. -d ./simple.tpl >> \\$\\(MAKEFILE)\n"
		"clean::\n"
		"\trm -f simple.c simple.tex\n"
		"simple.c simple.tex: ./simple.tpl\n"
		"	.*/mksource -I. ./simple.tpl\n"
		"tg: dep\n"
		"\tcmd\n"
		"# End of rules created with mksource\n"
	);
}

TEST_DECL(rules_advanced)
{
	RUN_MKSOURCE("./simple -r -H test.h -W work");
	LMATCH_CONTENT("stderr",
		"mksource -I. ./simple -r -H test.h -W work\n"
		"input < ./simple.tpl\n"
	);
	LMATCH_CONTENT("stdout",
		"\n"
		"# Rules created with\n"
		"# .*/mksource -I. ./simple -r -H test.h -W work\n"
		"all:: test.h work/simple.c work/simple.tex\n"
		"depend:: ./simple.tpl\n"
		"	.*/mksource -W work -I. -Htest.h -d ./simple.tpl >> \\$\\(MAKEFILE)\n"
		"clean::\n"
		"\trm -f test.h work/simple.c work/simple.tex\n"
		"test.h work/simple.c work/simple.tex: ./simple.tpl\n"
		"	.*/mksource -W work -I. -Htest.h ./simple.tpl\n"
		"tg: dep\n"
		"\tcmd\n"
		"# End of rules created with mksource\n"
	);
}

TEST_DECL(list)
{
	RUN_MKSOURCE("./simple -l");
	PRINT_CONTENT("stdout");
	LMATCH_CONTENT("stderr",
		"mksource -I. ./simple -l\n"
		"input < ./simple.tpl\n"
	);
	EXPECT_CONTENT("stdout",
		"simple.c\n"
		"simple.tex\n"
	);
}

TEST_DECL(depend)
{
	RUN_MKSOURCE("./simple -d");
	LMATCH_CONTENT("stderr",
		"mksource -I. ./simple -d\n"
		"input < ./simple.tpl\n"
	);
	EXPECT_CONTENT("stdout",
		"\n"
		"simple.c simple.tex: ./simple.tpl ./test.ms\n"
	);
}

TEST_DECL(std)
{
	RUN_MKSOURCE("./simple");

	LMATCH_CONTENT("stderr",
		"mksource -I. ./simple\n"
		"input < ./simple.tpl\n"
		"src > simple.c\n"
		"tex > simple.tex\n"
		"hdr == src\n"
	);

	LMATCH_CONTENT("simple.c",
		"/\\*\n"
		"Title\n"
		"Do not edit, file was created with\n"
		"mksource -I. ./simple\n"
		"\\*/\n"
		"Header\n"
		"/\\*\n"
		"Comment\n"
		"\\*/\n"
		"COMMENT: Comment\n"
		"Source\n"
		"/\\*\n"
		"END\n"
		"\\*/\n"
	);

	EXPECT_CONTENT("simple.tex",
		"% Title\n"
		"\n"
	);
}

TEST_DECL(advanced)
{
	TEST_DIR("work");
	RUN_MKSOURCE("-H test.h -W work ./simple");

	LMATCH_CONTENT("stderr",
		"mksource -I. -H test.h -W work ./simple\n"
		"hdr > test.h\n"
		"input < ./simple.tpl\n"
		"src > work/simple.c\n"
		"tex > work/simple.tex\n"
	);

	LMATCH_CONTENT("test.h",
		"/\\*\n"
		"Do not edit, file was created with\n"
		"mksource -I. -H test.h -W work ./simple\n"
		"\\*/\n"
		"#ifndef\tms_test_h\n"
		"#define\tms_test_h\t1\n"
		"/\\*\n"
		"Comment\n"
		"\\*/\n"
		"Header\n"
		"/\\*\n"
		"END\n"
		"\\*/\n"
		"#endif\t/\\* ms_test_h \\*/\n"
	);

	LMATCH_CONTENT("work/simple.c",
		"/\\*\n"
		"Title\n"
		"Do not edit, file was created with\n"
		"mksource -I. -H test.h -W work ./simple\n"
		"\\*/\n"
		"/\\*\n"
		"Comment\n"
		"\\*/\n"
		"COMMENT: Comment\n"
		"Source\n"
		"/\\*\n"
		"END\n"
		"\\*/\n"
	);

	EXPECT_CONTENT("work/simple.tex",
		"% Title\n"
		"\n"
	);
}

TEST_DECL(special)
{
	TEST_FILE("special.tpl",
		"config name=x hdr src\n"
	);
	RUN_MKSOURCE("./special -L list");
	EXPECT_CONTENT("list",
		"x.h\n"
		"x.c\n"
	);
}

TEST_SUITE
{
	TEST_RUN(setup);
	TEST_RUN(synopsis);
	TEST_RUN(rules);
	TEST_RUN(rules_advanced);
	TEST_RUN(list);
	TEST_RUN(depend);
	TEST_RUN(std);
	TEST_RUN(advanced);
	TEST_RUN(special);
}
