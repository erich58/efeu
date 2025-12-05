#include <EFEU/UnitTest.h>
#include <EFEU/EfiClass.h>
#include <EfiType/AGE99.h>
#include <EFEU/utest/term.tst.h>

TEST_DECL(conv)
{
	EXPECT_TERM("AGE99(-1)", "AGE99: 00", NULL);
	EXPECT_TERM("AGE99(0)", "AGE99: 00", NULL);
	EXPECT_TERM("AGE99(50)", "AGE99: 50", NULL);
	EXPECT_TERM("AGE99(99)", "AGE99: 99", NULL);
	EXPECT_TERM("AGE99(100)", "AGE99: 99", NULL);
	EXPECT_TERM("(int) AGE99(50)", "int: 50", NULL);

	EXPECT_TERM("AGE99(NULL)", "AGE99: 00", NULL);
	EXPECT_TERM("AGE99(\"50\")", "AGE99: 50", NULL);
	EXPECT_TERM("AGE99(\"A50\")", "AGE99: 50", NULL);
	EXPECT_TERM("(str) AGE99(50)", "str: \"A50\"", NULL);
}

TEST_DECL(class)
{
	MakeEfiClass(&Type_AGE99, "c1", "cut=20,30,50");
	EXPECT_TERM("(int) AGE99(0).c1", "int: 1", NULL);
	EXPECT_TERM("(int) AGE99(20).c1", "int: 1", NULL);
	EXPECT_TERM("(int) AGE99(21).c1", "int: 2", NULL);
	EXPECT_TERM("(int) AGE99(30).c1", "int: 2", NULL);
	EXPECT_TERM("(int) AGE99(31).c1", "int: 3", NULL);
	EXPECT_TERM("(int) AGE99(50).c1", "int: 3", NULL);
	EXPECT_TERM("(int) AGE99(51).c1", "int: 4", NULL);
	EXPECT_TERM("(int) AGE99(99).c1", "int: 4", NULL);
	EXPECT_TERM("typeof(AGE99::c1)",
		"Type_t: _enum_A00_20_1_A21_30_2_A31_50_3_A51_99_4_", NULL);
}

TEST_SUITE
{
	SetupStd();
	AGE99_setup();
	EXPECT(GetType("AGE99") == &Type_AGE99);

	TEST_RUN(conv);
	TEST_RUN(class);
	CLEANUP_TERM;
}
