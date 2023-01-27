#include <EFEU/UnitTest.h>
#include <EFEU/object.h>
#include <EFEU/EDB.h>
#include <EFEU/EDBMeta.h>
#include <EFEU/EDBFilter.h>
#include "utest/in.tst.h"
#include "utest/out.tst.h"

static void meta_test (EDBMetaDef *def, EDBMeta *meta, const char *arg)
{
	io_xprintf(ioerr, "%s: %#s\n", def->name, arg);
	io_xprintf(ioerr, "%#s\n", EDBMeta_par(meta, 1));
	io_close(meta->ctrl);
	meta->ctrl = NULL;
}

static EDBMetaDef mdef = { "meta_test", meta_test, 0,
	"Description name=$1 ($$-$2-$x)"
};

TEST_DECL(setup)
{
	setenv("LANG", "C", 1);
	SetProgName("test");
	SetApplPath(UnitTest_tmpname(NULL));

	TEST_FILE("edb.msg",
		"mhdr	$!: wrong magic number.\n"
		"type	$!: data type not defined.\n"
		"cmd	$!: unknown command $1.\n"
		"uchar	$!: unexpected character $1.\n"
	);

	SetupStd();
	AddEDBMetaDef(&mdef, 1);
	SetupEDBFilter();
}

TEST_DECL(show)
{
	InfoNode root = STD_INFO(NULL, NULL);
	EDBMetaInfo(&root);
	InfoNode *info;
	EXPECT_PTR(info = GetInfo(&root, "meta"));
	PrintInfo(OUT(), info);
	MATCH_OUT(".");

	EDBMetaDef *mdef;
	EXPECT_PTR(mdef = GetEDBMetaDef("meta_test"));
	ShowEDBMetaDef(OUT(), mdef);
	EXPECT_OUT("meta_test\tDescription name=meta_test ($-2-x)\n");
}

TEST_DECL(meta)
{
	UnitTest_stderr("stderr");
	EXPECT(edb_meta(rd_refer(IN("Bad header\n")), NULL) == NULL);
	UnitTest_stderr(NULL);
	EXPECT_CONTENT("stderr", "ERR test: wrong magic number.\n");

	UnitTest_stderr("stderr");
	EXPECT(edb_meta(rd_refer(IN(
		"EDB 1.0\n"
		"@data\n"
	)), NULL) == NULL);
	UnitTest_stderr(NULL);
	EXPECT_CONTENT("stderr", "ERR test: data type not defined.\n");

	UnitTest_stderr("stderr");
	EXPECT(edb_meta(rd_refer(IN(
		"EDB 1.0\n"
		"// DESC\n"
		"int\n"
		"@undef\n"
		"@end\n"
		"x\n"
	)), NULL) == NULL);
	UnitTest_stderr(NULL);
	EXPECT_CONTENT("stderr",
		"ERR test: unknown command undef.\n"
		"ERR test: unexpected character 'x'.\n"
		);

	UnitTest_stderr("stderr");
	EXPECT(edb_meta(rd_refer(IN(
		"EDB 1.0\n"
		"// DESC\n"
		"@meta_test Arg\\umen/**/t-\\\n"
		" /*xx*/$!  \n"
		"Parameter $!\n"
		"Zeile 2\n"
		"@end\n"
		"something\n"
	)), NULL) == NULL);
	UnitTest_stderr(NULL);
	EXPECT_CONTENT("stderr",
		"meta_test: \"Arg\\\\ument-test\"\n"
		"\"Parameter test\\nZeile 2\"\n"
		);
}

static const char *copy_db(const char *in, const char *out)
{
	UnitTest_mkfile(out, NULL);
	EDB *edb = edb_fopen(NULL, UnitTest_tmpname(in));
	edb_fout(edb, UnitTest_tmpname(out), "plain");
	edb_copy(edb, 0);
	rd_deref(edb);
	return UnitTest_content(out);
}

#define	EXPECT_DB(name, cont)	\
UnitTest_strcheck(__FILE__, __LINE__, "==", \
	copy_db(name, "output"), cont, "edb(" # name ")", # cont)


TEST_DECL(file)
{
	TEST_FILE("db",
		"EDB\t1.0\n"
		"// DESC\n"
		"int\n"
		"@file\n"
	);
	EXPECT_DB("db",
		"EDB\t1.0\n"
		"// DESC\n"
		"int\n"
		"@data plain\n"
	);

	TEST_FILE("db1",
		"EDB\t1.0\n"
		"// DESC 1\n"
		"int\n"
		"@data plain\n"
		"15\n"
		"3\n"
	);
	TEST_FILE("db2",
		"EDB\t1.0\n"
		"// DESC 2\n"
		"int\n"
		"@data plain\n"
		"7\n"
		"5\n"
	);
	TEST_FILE("db",
		"EDB\t1.0\n"
		"// DESC\n"
		"int\n"
		"@file\n"
		"db1\n"
		"@filter\n"
		"var=a=this,b=1\n"
	);
	EXPECT_DB("db",
		"EDB\t1.0\n"
		"// DESC\n"
		"int a;\n"
		"int b;\n"
		"@data plain\n"
		"#a;b\n"
		"15;1\n"
		"3;1\n"
	);

	TEST_FILE("db",
		"EDB\t1.0\n"
		"int\n"
		"@file sort\n"
		"db1\n"
	);
	EXPECT_DB("db",
		"EDB\t1.0\n"
		"// DESC 1\n"
		"int\n"
		"@data plain\n"
		"3\n"
		"15\n"
	);

	TEST_FILE("db",
		"EDB\t1.0\n"
		"@file\n"
		"db1\n"
		"db2\n"
	);
	EXPECT_DB("db",
		"EDB\t1.0\n"
		"int\n"
		"@data plain\n"
		"15\n"
		"3\n"
		"7\n"
		"5\n"
	);

	TEST_FILE("db",
		"EDB\t1.0\n"
		"// DESC\n"
		"int\n"
		"@file sort=+\n"
		"db1\n"
		"db2\n"
	);
	EXPECT_DB("db",
		"EDB\t1.0\n"
		"// DESC\n"
		"int\n"
		"@data plain\n"
		"3\n"
		"5\n"
		"7\n"
		"15\n"
	);

	TEST_FILE("db",
		"EDB\t1.0\n"
		"// DESC\n"
		"int\n"
		"@file merge=-\n"
		"db1\n"
		"db2\n"
	);
	EXPECT_DB("db",
		"EDB\t1.0\n"
		"// DESC\n"
		"int\n"
		"@data plain\n"
		"15\n"
		"7\n"
		"5\n"
		"3\n"
	);

	TEST_FILE("db",
		"EDB\t1.0\n"
		"@file sort\n"
		"db1\n"
		"@file sort\n"
		"db2\n"
		"@paste\n"
	);
	EXPECT_DB("db",
		"EDB\t1.0\n"
		"// DESC 1\n"
		"int\n"
		"@data plain\n"
		"3\n"
		"15\n"
		"5\n"
		"7\n"
	);
	TEST_FILE("db",
		"EDB\t1.0\n"
		"@file sort\n"
		"db1\n"
		"@file sort\n"
		"db2\n"
		"@paste merge\n"
	);
	EXPECT_DB("db",
		"EDB\t1.0\n"
		"// DESC 1\n"
		"int\n"
		"@data plain\n"
		"3\n"
		"5\n"
		"7\n"
		"15\n"
	);
}

TEST_DECL(head)
{
	TEST_FILE("db",
		"EDB\t1.0\n"
		"@head\n"
		"fprintf(ioerr, \"%d\\n\",  6 * 7);\n"
		"@end\n"
	);
	UnitTest_stderr("stderr");
	EXPECT_DB("db", NULL);
	UnitTest_stderr(NULL);
	EXPECT_CONTENT("stderr", "42\n");
}

TEST_DECL(expr)
{
	TEST_FILE("db",
		"EDB\t1.0\n"
		"@type\n"
		"int a;\n"
		"@expr\n"
		"static int n = 5;\n"
		"if (n == 0) break;\n"
		"a = n--;\n"
		"if (a == 3) continue;\n"
		"@end\n"
	);
	EXPECT_DB("db", 
		"EDB\t1.0\n"
		"int a;\n"
		"@data plain\n"
		"#a\n"
		"5\n"
		"4\n"
		"2\n"
		"1\n"
	);

	edb_expr(NULL, NULL);	// Coverage
}

TEST_DECL(join)
{
	TEST_FILE("db1",
		"EDB\t1.0\n"
		"int a;\n"
		"int b;\n"
		"@data plain\n"
		"#a;b\n"
		"3;1\n"
		"5;2\n"
		"15;3\n"
	);
	TEST_FILE("db2",
		"EDB\t1.0\n"
		"int a;\n"
		"int c;\n"
		"@data plain\n"
		"#a;c\n"
		"3;91\n"
		"15;92\n"
	);
	TEST_FILE("db",
		"EDB\t1.0\n"
		"@file\n"
		"db1\n"
		"@file\n"
		"db2\n"
		"@join\n"
	);
	EXPECT_DB("db",
		"EDB\t1.0\n"
		"int a;\n"
		"int b;\n"
		"int c;\n"
		"@data plain\n"
		"#a;b;c\n"
		"3;1;91\n"
		"15;3;92\n"
	);
}

TEST_DECL(import)
{
	TEST_FILE("data", "\001\002\003\004");
	TEST_FILE("data1",
		"3\n"
		"5\n"
		"15\n"
	);
	TEST_FILE("data2",
		"11\n"
		"12\n"
	);
	UnitTest_stdin("stdin", "42\n");
	TEST_FILE("db",
		"EDB\t1.0\n"
		"int\n"
		"@import binary\n"
		"data\n"
		"@import plain\n"
		"|echo 99\n"
		"data1\n"
		"-\n"
		"data2\n"
	);
	EXPECT_DB("db",
		"EDB\t1.0\n"
		"int\n"
		"@data plain\n"
		"16909060\n" /* 0x1020304 */
		"99\n"
		"3\n"
		"5\n"
		"15\n"
		"42\n"
		"11\n"
		"12\n"
	);
	UnitTest_stdin(NULL, NULL);
}

TEST_SUITE
{
	TEST_RUN(setup);
	TEST_RUN(show);
	TEST_RUN(meta);
	TEST_RUN(file);
	TEST_RUN(head);
	TEST_RUN(expr);
	TEST_RUN(join);
	TEST_RUN(import);
	CLEANUP_IN;
	CLEANUP_OUT;
}
