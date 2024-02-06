#include <EFEU/UnitTest.h>
#include <EFEU/mdcount.h>
#include <EFEU/locale.h>
//#include <EFEU/EfiCount.h>

TEST_DECL(setup)
{
	SetProgName("ID");
	SetLocale(LOC_DATE, "C");
	SetApplPath(UnitTest_tmp(NULL));
	SetupStd();
//	SetupEfiCount();
	TEST_FILE("mdmat.msg",
		"302	$!: classification table is not initialized.\n"
	);
}

static Label test_label[] = {
	{ "a", "Label a", },
	{ "b", "Label b", },
};

static MdClass test_class = {
	"test", "Testklassifikation",
	2, test_label,
	NULL,
};

static MdClass test_class_table[] = {
	{ "t1", "Class 1", 2, test_label, NULL, },
	{ "t2", "Class 2", 2, test_label, NULL, },
};

TEST_SUITE
{
	TEST_RUN(setup);
	UnitTest_stderr("stderr");
	MdClass_add(NULL, NULL, 0);
	UnitTest_stderr(NULL);
	EXPECT_CONTENT("stderr",
		"MSG ID: classification table is not initialized.\n"
	);

	MdCountPar *par = MdCountPar_create();
	AddMdClass(par, &test_class);
	EXPECT_UINT(par->ctab.used, 1);
	MdClass_add(par, test_class_table, 2);
	EXPECT_UINT(par->ctab.used, 3);

	EXPECT(MdClass_get(par, NULL) == NULL);
	EXPECT(MdClass_get(par, "test") == &test_class);
}

