#include <EFEU/UnitTest.h>
#include <stdlib.h>

int hidden_main(int argc, char **argv);

#define	SRC2DOC "./src2doc"

#define	RUN_SRC2DOC(n, def)	\
TEST_FORK { \
	TEST_MKARG(rc, UnitTest_tpl(SRC2DOC " " def)); \
	UnitTest_reopen(1, "stdout"); \
	UnitTest_reopen(2, "stderr"); \
	EXPECT(hidden_main(rc->argc, rc->argv) == 0); \
	TEST_EXIT; \
} CHILD_EXIT(0)


TEST_DECL(setup)
{
	setenv("LANG", "C", 1);
	PRINT_LABEL("Force separating UnitTest output from cmd output");
	UnitTest_stderr("stderr");
	UnitTest_stderr(NULL);
}

TEST_DECL(synopsis)
{
	TEST_FORK
	{
		UnitTest_mkcmd("efeudoc", "#!/bin/sh\ncat\n");
		UnitTest_reopen(2, "stderr");
		TEST_MKARG(rc, UnitTest_tpl(SRC2DOC)); \
		EXPECT(hidden_main(rc->argc, rc->argv) == 1);
		TEST_EXIT;
	}

	CHILD_EXIT(1);
	EXPECT_CONTENT("stderr",
		"\\Synopsis\n"
		"\\hang\n"
		"|src2doc|\n"
		"\\[~|--help|[|=|</type/>]~]\n"
		"\\[~|--version|~]\n"
		"\\[~|--info|[|=|</entry/>]~]\n"
		"\\[~|--debug|[|=|</mode/>]~]\n"
		"\\[~|--verbose|~]\n"
		"\\[~|-t|~]\n"
		"\\[~|--ps|~]\n"
		"\\[~|-c|~]\n"
		"\\[~|-n|~</name/>~]\n"
		"\\[~|-m|~</mode/>~]\n"
		"\\[~|-s|~</num/>~]\n"
		"\\[~|-i|~</fmt/>~]\n"
		"\\[~|-g|~]\n"
		"\\[~|-L|~</lang/>~]\n"
		"\\[~|-l|~]\n"
		"\\[~|-a|~]\n"
		"</src/>\n"
		"\\[~</out/>~]\n"
		"\\end\n"
	);
}

TEST_SUITE
{
	TEST_RUN(setup);
	TEST_RUN(synopsis);
}
