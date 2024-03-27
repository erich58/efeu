#include <EFEU/UnitTest.h>
#include <EFEU/mstring.h>
#include <stdlib.h>
#include <EFEU/procenv.h>
#include <EFEU/mkpath.h>

int hidden_main(int argc, char **argv);

#define	EFEUDOC TOP "/bin/efeudoc"

#define	RUN_EFEUDOC(def)	\
TEST_FORK { \
	TEST_MKARG(rc, EFEUDOC " " def); \
	UnitTest_reopen(1, "stdout"); \
	EXPECT(hidden_main(rc->argc, rc->argv) == 0); \
	TEST_EXIT; \
} CHILD_EXIT(0)

TEST_DECL(setup)
{
	setenv("LANG", "C", 1);
	char *path = mstrcat(TOP "/bin:", UnitTest_tpl("@/bin:"),
		getenv("PATH"), NULL);
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
		TEST_MKARG(rc, EFEUDOC);
		EXPECT(hidden_main(rc->argc, rc->argv) == 1);
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

TEST_DECL(simple)
{
	TEST_DIR("etc");
	TEST_FILE("etc/efeudoc.rc", "\n");
	TEST_FILE("doc.eds",
		"\\chap Ultimate Question:\n"
		"The answer is \\eval{6*7}.\n");
	RUN_EFEUDOC("-H 'Headline' -o @/doc.out @/doc.eds");
	EXPECT_CONTENT("doc.out",
		"Headline\n"
		"\n"
		"Ultimate Question:\n"
		"\n"
		"The answer is 42.\n"
	);
}

TEST_DECL(rules)
{
	RUN_EFEUDOC("-H Headline -Tterm -r target -o doc.out /dev/null");
	LMATCH_CONTENT("stdout",
		"target:: doc.out\n"
		"\n"
		"doc.out: \n"
		"\tLANG=C .*/efeudoc -H \"Headline\" -T term -o doc.out /dev/null"
	);
}

TEST_DECL(depend)
{
	RUN_EFEUDOC("-H 'Headline' -d -M @/depend -o doc.out /dev/null");
	LMATCH_CONTENT("stdout",
		"doc.out: \n"
	);
	LMATCH_CONTENT("depend",
		"doc.out: \n"
	);
}

TEST_SUITE
{
	TEST_RUN(setup);
	TEST_RUN(synopsis);
	TEST_RUN(simple);
	TEST_RUN(rules);
	TEST_RUN(depend);
}
