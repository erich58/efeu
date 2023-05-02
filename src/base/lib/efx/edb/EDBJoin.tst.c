#include <EFEU/UnitTest.h>
#include <EFEU/object.h>
#include <EFEU/EDB.h>
#include <EFEU/EDBMeta.h>
#include <EFEU/EDBJoin.h>
#include "utest/out.tst.h"

static const char *copy_db(const char *in, const char *out)
{
	UnitTest_mkfile(out, NULL);
	EDB *edb = edb_fopen(NULL, UnitTest_tmp(in));
	edb_fout(edb, UnitTest_tmp(out), "plain");
	edb_copy(edb, 0);
	rd_deref(edb);
	return UnitTest_content(out);
}

#define	EXPECT_DB(name, cont)	\
UnitTest_strcheck(__FILE__, __LINE__, "==", \
	copy_db(name, "output"), cont, "edb(" # name ")", # cont)

static int jtest_read (EfiType *type, void *data, void *par)
{
	EDBJoinPar *jpar = par;
	int *i1 = edb_read(jpar->db1);
	int *i2 = edb_read(jpar->db2);
	Val_int(data) = 0;

	if	(i1)
		Val_int(data) += *i1 * 1000;

	if	(i2)
		Val_int(data) += *i2;
 
	return (i1 || i2) ? 1 : 0;
}

static EDB *jtest_create (EDBJoin *def, EDB *db1, EDB *db2, const char *arg)
{
	EDB *edb = edb_create(def->type);
	edb->read = jtest_read;
	edb->ipar = EDBJoinPar_create(db1, db2);
	return edb;
}

TEST_DECL(setup)
{
	static EDBJoin jpar;
	setenv("LANG", "C", 1);
	SetProgName("test");
	SetApplPath(UnitTest_tmp(NULL));

	TEST_FILE("edb.msg",
		"mhdr	$!: wrong magic number.\n"
		"type	$!: data type not defined.\n"
		"cmd	$!: unknown command $1.\n"
		"uchar	$!: unexpected character $1.\n"
	);

	SetupStd();
	jpar.t1 = str2Type("typedef int T1");
	jpar.t2 = str2Type("typedef int T2");
	jpar.type = &Type_int;
	jpar.create = jtest_create;
	jpar.par = "jpar";
	jpar.desc = "Join Test";
	AddEDBJoin(&jpar);
	EXPECT(GetEDBJoin(jpar.t1, jpar.t2) == &jpar);
}

TEST_DECL(list)
{
	InfoNode root = STD_INFO(NULL, NULL);
	EDBJoinInfo(&root);
	InfoNode *info;
	EXPECT_PTR(info = GetInfo(&root, "join"));
	PrintInfo(OUT(), info);
	MATCH_OUT(".");
}

TEST_DECL(test_join)
{
	TEST_FILE("db1",
		"EDB\t1.0\n"
		"T1\n"
		"@data plain\n"
		"3\n"
		"5\n"
		"7\n"
	);
	TEST_FILE("db2",
		"EDB\t1.0\n"
		"T2\n"
		"@data plain\n"
		"91\n"
		"92\n"
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
		"int\n"
		"@data plain\n"
		"3091\n"
		"5092\n"
		"7000\n"
	);
}

TEST_SUITE
{
	TEST_RUN(setup);
	TEST_RUN(list);
	TEST_RUN(test_join);
	CLEANUP_OUT;
}
