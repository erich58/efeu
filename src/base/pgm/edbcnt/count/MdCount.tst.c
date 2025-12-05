#include <EFEU/UnitTest.h>
#include "mdcount.h"
#include <EFEU/locale.h>
#include <EFEU/utest/out.tst.h>

TEST_DECL(setup)
{
	SetProgName("ID");
	SetLocale(LOC_DATE, "C");
	SetApplPath(UnitTest_tmp(NULL));
	SetupStd();
	TEST_FILE("mdmat.msg",
		"301	$!: count table is not initialized.\n"
	);
}

TEST_DECL(uninitialized)
{
	UnitTest_stderr("stderr");
	MdCount_add(NULL, NULL, 0);
	UnitTest_stderr(NULL);
	EXPECT_CONTENT("stderr",
		"MSG ID: count table is not initialized.\n"
	);
	EXPECT(MdCount_get(NULL, "count") == NULL);
}

TEST_DECL(std)
{
	InfoNode info = STD_INFO(NULL, NULL);
	MdCountPar *par = MdCountPar_create();
	EXPECT_UINT(par->vtab.used, 2);

	md_showcnt(OUT(), par);
	EXPECT_OUT(
		"count\tstandard counter\n"
		"wcount\tweighted count\n"
	);

	EXPECT(MdCount_get(par, "count") == stdcount);

	MdCountInfo(&info, par);
	PrintInfo(OUT(), &info);
	EXPECT_OUT(
		"\n"
		"count\tZählerdefinitionen\n"
	);
	PrintInfo(OUT(), rd_deref(GetInfo(&info, "count/count")));
	EXPECT_OUT(
		"standard counter\n"
		"int\n"
	);
	PrintInfo(OUT(), rd_deref(GetInfo(&info, "count/wcount")));
	EXPECT_OUT(
		"weighted count\n"
		"double\n"
	);
}

static void add (MdCount *cnt, void *data, void *buf)
{
	Val_double(data)++;
}

TEST_DECL(extension)
{
	static MdCount dcount = {
		"dcount",
		"double",
		"double counter",
		NULL,
		add,
	};

	MdCountPar *par = MdCountPar_create();
	AddMdCount(par, &dcount); 

	md_showcnt(OUT(), par);
	EXPECT_OUT(
		"count\tstandard counter\n"
		"dcount\tdouble counter\n"
		"wcount\tweighted count\n"
	);

	EXPECT(MdCount_get(par, "dcount") == &dcount);
	rd_deref(par);
}

TEST_SUITE
{
	TEST_RUN(setup);
	TEST_RUN(uninitialized);
	TEST_RUN(std);
	TEST_RUN(extension);

	CLEANUP_OUT;
}
