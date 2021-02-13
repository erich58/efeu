#include <EFEU/UnitTest.h>
#include <EFEU/object.h>
#include <EFEU/EDB.h>
#include <EFEU/EDBMeta.h>
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
	EXPECT_CONTENT("stderr", "test: wrong magic number.\n");

	UnitTest_stderr("stderr");
	EXPECT(edb_meta(rd_refer(IN(
		"EDB 1.0\n"
		"@data\n"
	)), NULL) == NULL);
	UnitTest_stderr(NULL);
	EXPECT_CONTENT("stderr", "test: data type not defined.\n");

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
		"test: unknown command undef.\n"
		"test: unexpected character 'x'.\n"
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
	);
	EXPECT_DB("db",
		"EDB\t1.0\n"
		"// DESC\n"
		"int\n"
		"@data plain\n"
		"15\n"
		"3\n"
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

TEST_SUITE
{
	TEST_RUN(setup);
	TEST_RUN(show);
	TEST_RUN(meta);
	TEST_RUN(file);
	CLEANUP_IN;
	CLEANUP_OUT;
}
