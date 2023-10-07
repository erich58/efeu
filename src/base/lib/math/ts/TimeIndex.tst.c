#include <EFEU/UnitTest.h>
#include <EFEU/object.h>
#include <Math/TimeSeries.h>

TEST_DECL(setup)
{
	SetupStd();
	SetProgName("ID");
	SetApplPath(UnitTest_tmp(NULL));
	TEST_FILE("TimeSeries.msg",
		"2	$!: unknown index type '$1'.\n"
	);
}

TEST_DECL(index)
{
	EXPECT_INT(tindex_type("jear"), TS_YEAR);
	EXPECT_INT(tindex_type("quart"), TS_QUART);
	EXPECT_INT(tindex_type("month"), TS_MONTH);
	EXPECT_INT(tindex_type("week"), TS_WEEK);
	EXPECT_INT(tindex_type("day"), TS_DAY);

	UnitTest_stderr("err");
	EXPECT_INT(tindex_type("x"), 0);
	UnitTest_stderr(NULL);
	EXPECT_CONTENT("err", "MSG ID: unknown index type 'x'.\n");
}

TEST_SUITE
{
	TEST_RUN(setup);
	TEST_RUN(index);
}
