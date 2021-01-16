#include <EFEU/UnitTest.h>
#include <EFEU/object.h>
#include <EFEU/EDB.h>
#include <EFEU/EfiOutput.h>
#include "utest/out.tst.h"

TEST_DECL(psub)
{
	EXPECT_STR(EDBPrintDef_psub("name[]=#", "opt", "arg"), "name[opt]=arg");
	EXPECT_STR(EDBPrintDef_psub("name[]=#", NULL, "arg"), "name=arg");
	EXPECT_STR(EDBPrintDef_psub("name[]=#", NULL, NULL), "name");
}

TEST_DECL(show)
{
	static EDBPrintDef pdef = { "test", "[opt]=arg", EDBPrint_null,
		"data=#", "Description name=$1\nDetails: ($2), $$-$3-$x"
	};

	ShowEDBPrintDef(OUT(), &pdef, 0);
	EXPECT_OUT("test\tDescription name=test\n");
	ShowEDBPrintDef(OUT(), &pdef, 1);
	EXPECT_OUT(
		"Description name=test\n"
		"\n"
		"Syntax: test[opt]=arg\n"
		"\n"
		"Details: (data=arg), $-3-x\n"
	);
}

TEST_DECL(list)
{
	InfoNode root = STD_INFO(NULL, NULL);
	EDBPrintInfo(&root);
	InfoNode *info;
	EXPECT_PTR(info = GetInfo(&root, "print"));
	PrintInfo(OUT(), info);
	MATCH_OUT(".");

	EDBPrintDef *pdef;
	EXPECT_PTR(pdef = GetEDBPrintDef("data"));
	ShowEDBPrintDef(OUT(), pdef, 0);
	EXPECT_OUT("data\tdata without declarations\n");
	ShowEDBPrintDef(OUT(), pdef, 1);
	EXPECT_OUT(
		"data without declarations\n"
		"\n"
		"Syntax: data[opt]=def\n"
	);
}

TEST_DECL(pmode)
{
	EDBPrintMode *pmode;
	EDB *edb = edb_create(&Type_int);
	EXPECT_PTR(pmode = edb_pmode(NULL, edb, NULL));
	EXPECT_STR(pmode->name, "plain");
	EXPECT(pmode->init != NULL);
	EXPECT(pmode->header == 2);
	EXPECT(pmode->filter == NULL);
	EXPECT(pmode->split == 512 * 1024ul * 1024ul);
	EXPECT(pmode->par == NULL);

	EXPECT_PTR(pmode = edb_pmode(NULL, edb, "test[opt]=arg"));
	EXPECT(pmode->par != NULL);

	UnitTest_stderr("stderr");
	TEST_FORK
	{
		edb_pmode(NULL, edb, "default=?");
	}
	CHILD_EXIT(0);
	UnitTest_stderr(NULL);
	EXPECT_CONTENT("stderr",
		"default flags: plain,verbosity=2,split=512M\n"
		"\n"
		"Syntax: default\n"
	);

	EXPECT_PTR(pmode = edb_pmode(NULL, edb, "locale=de_AT"));
}

TEST_SUITE
{
	TEST_RUN(psub);
	TEST_RUN(show);
	TEST_RUN(list);
	TEST_RUN(pmode);
	CLEANUP_OUT;
}
