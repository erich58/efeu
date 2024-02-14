#include <EFEU/UnitTest.h>
#include <EFEU/mdcount.h>
#include <EFEU/locale.h>
#include "utest/out.tst.h"

TEST_DECL(setup)
{
	SetProgName("ID");
	SetLocale(LOC_DATE, "C");
	SetApplPath(UnitTest_tmp(NULL));
	SetupStd();
	TEST_FILE("mdmat.msg",
		"302	$!: classification table is not initialized.\n"
	);
}

TEST_DECL(uninitialized)
{
	UnitTest_stderr("stderr");
	MdClass_add(NULL, NULL, 0);
	UnitTest_stderr(NULL);
	EXPECT_CONTENT("stderr",
		"MSG ID: classification table is not initialized.\n"
	);
}

static Label test_label[] = {
	{ "a", "Label a", },
	{ "b", "Label b", },
	{ "c", "Label c", },
	{ "d", "Label d", },
	{ "e", "Label e", },
};

static MdClass test_class = {
	"test", "Testklassifikation",
	5, test_label,
	NULL,
};

static MdClass test_class_table[] = {
	{ "t1", "Class 1", 2, test_label, NULL, },
	{ "t2", "Class 2", 3, test_label + 2, NULL, },
};

TEST_SUITE
{
	TEST_RUN(setup);
	TEST_RUN(uninitialized);

	MdCountPar *par = MdCountPar_create();
	EXPECT_UINT(par->ctab.used, 0);

	AddMdClass(par, &test_class);
	EXPECT_UINT(par->ctab.used, 1);
	MdClass_add(par, test_class_table, 2);
	EXPECT_UINT(par->ctab.used, 3);

	EXPECT(MdClass_get(par, NULL) == NULL);
	EXPECT(MdClass_get(par, "test") == &test_class);
	
	MdClassList(OUT(), &test_class);
	EXPECT_OUT("test[5]\tTestklassifikation\n");

	MdClassPrint(OUT(), &test_class);
	EXPECT_OUT(
		"test[5]\tTestklassifikation\n"
		"\ta\tLabel a\n"
		"\tb\tLabel b\n"
		"\tc\tLabel c\n"
		"\td\tLabel d\n"
		"\te\tLabel e\n"
	);

	MdClassPrintLimit = 2;
	MdClassPrint(OUT(), &test_class);
	EXPECT_OUT(
		"test[5]\tTestklassifikation\n"
		"\ta\tLabel a\n"
		"\tb\tLabel b\n"
		"\t...\t...\n"
		"\td\tLabel d\n"
		"\te\tLabel e\n"
	);

	MdClassPrintLimit = 40;
	MdShowClass(OUT(), par, NULL);
	EXPECT_OUT(
		"t1[2]\tClass 1\n"
		"t2[3]\tClass 2\n"
		"test[5]\tTestklassifikation\n"
	);

	MdShowClass(OUT(), par, "t1,test");
	EXPECT_OUT(
		"t1[2]\tClass 1\n"
		"\ta\tLabel a\n"
		"\tb\tLabel b\n"
		"test[5]\tTestklassifikation\n"
		"\ta\tLabel a\n"
		"\tb\tLabel b\n"
		"\tc\tLabel c\n"
		"\td\tLabel d\n"
		"\te\tLabel e\n"
	);

	MdClassInfo(NULL, par);
	DumpInfo(OUT(), NULL, "class", 0);
	EXPECT_OUT(
		"@: Selektionsklassen\n"
		"@t1|ID --info=dump:/class/t1\n"
		"@t2|ID --info=dump:/class/t2\n"
		"@test|ID --info=dump:/class/test\n"
	);

	PrintInfo(OUT(), rd_deref(GetInfo(NULL, "class/t2")));
	EXPECT_OUT(
		"Class 2\n"
		"c\tLabel c\n"
		"d\tLabel d\n"
		"e\tLabel e\n"
	);

	CLEANUP_OUT;
}
