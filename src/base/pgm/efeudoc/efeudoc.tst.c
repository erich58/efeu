#include <EFEU/UnitTest.h>
#include <EFEU/mstring.h>
#include <stdlib.h>

int hidden_main(int argc, char **argv);

//#define	EFEUDOC SRC "/efeudoc"
#define	EFEUDOC TOP "/bin/efeudoc"

/*
#define	RUN_EFEUDOC(n, def)	\
TEST_FORK { \
	TEST_ARGV(main_args, n + 1, UnitTest_tpl(EFEUDOC " " def)); \
	UnitTest_reopen(1, "stdout"); \
	UnitTest_reopen(2, "stderr"); \
	EXPECT(hidden_main(n + 1, main_args) == 0); \
	TEST_EXIT; \
} CHILD_EXIT(0)
*/

#define	RUN_EFEUDOC(n, def)	\
do { \
	TEST_ARGV(main_args, n + 1, UnitTest_tpl(EFEUDOC " " def)); \
	UnitTest_stdout("stdout"); \
	UnitTest_stderr("stderr"); \
	EXPECT(hidden_main(n + 1, main_args) == 0); \
	UnitTest_stdout(NULL); \
	UnitTest_stderr(NULL); \
} while(0)

TEST_DECL(setup)
{
	setenv("LANG", "C", 1);
	char *path = mstrpaste(":", TOP "/bin", getenv("PATH"));
	setenv("PATH", path, 1);
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
		char *args[] = { EFEUDOC, NULL };
		EXPECT(hidden_main(1, args) == 1);
		TEST_EXIT;
	}

	CHILD_EXIT(1);
	EXPECT_CONTENT("stderr",
		"\\Synopsis\n"
		"\\hang\n"
		"|efeudoc|\n"
		"\\[~|--help|[|=|</type/>]~]\n"
		"\\[~|--version|~]\n"
		"\\[~|--info|[|=|</entry/>]~]\n"
		"\\[~|--debug|[|=|</mode/>]~]\n"
		"\\[~|--verbose|~]\n"
		"\\[~|-I|~</dir/>~]\n"
		"\\[~|-T|~</type/>~]\n"
		"\\[~|-C|~</cfg/>~]\n"
		"\\[~|-H|~</hdr/>~]\n"
		"\\[~|-0|~]\n"
		"\\[~|-u|~]\n"
		"\\[~|-r|~</name/>~]\n"
		"\\[~|-d|~]\n"
		"\\[~|-M|~</file/>~]\n"
		"\\[~|-t|~]\n"
		"\\[~|-l|~]\n"
		"\\[~|-p|~]\n"
		"\\[~|--pdf|~]\n"
		"\\[~|-h|~]\n"
		"\\[~|-o|~</aus/>~]\n"
		"</file(s)/>\n"
		"\\end\n"
	);
}

extern char *DocPath;
extern char *CFGPath;
extern char *ApplPath;

TEST_DECL(null)
{
	RUN_EFEUDOC(6, "--debug=file.debug -T html -o x /dev/null");
	PRINT_STR(UnitTest_tmp(NULL));
	PRINT_STR(ApplPath);
	PRINT_STR(CFGPath);
	PRINT_STR(DocPath);
	system("rm -rf x");
}

TEST_SUITE
{
	TEST_RUN(setup);
	TEST_RUN(synopsis);
	TEST_RUN(null);
}
