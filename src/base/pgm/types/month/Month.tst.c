#include <EFEU/UnitTest.h>
#include <EfiType/Month.h>
#include <EFEU/utest/term.tst.h>

TEST_DECL(conv)
{
	EXPECT_TERM("Month m", "Month: NULL", NULL);
	EXPECT_TERM("m.year", "int: 0", NULL);
	EXPECT_TERM("m.month", "int: 0", NULL);
	EXPECT_TERM("m.year = 2020", "int: 2020", NULL);
	EXPECT_TERM("m", "Month: \"2020-00\"", NULL);
	EXPECT_TERM("m.month = 6", "int: 6", NULL);
	EXPECT_TERM("m", "Month: \"2020-06\"", NULL);

	EXPECT_TERM("m = \"2022-09-01\"", "Month: \"2022-09\"", NULL);
	EXPECT_TERM("m.year", "int: 2022", NULL);
	EXPECT_TERM("m.month", "int: 9", NULL);

	EXPECT_TERM("m = Date(\"2002-01-01\")", "Month: \"2002-01\"", NULL);
	EXPECT_TERM("(Date) m", "Date: Date(01,01,2002)", NULL);

	EXPECT_TERM("m = { 2003, 7 }", "Month: \"2003-07\"", NULL);
	EXPECT_TERM("(List_t) m", "List_t: {2003, 7}", NULL);
}

TEST_DECL(obj)
{
	EfiObj *obj = EvalObj(str2Obj("2000-03-12"), &Type_Month);
	Month *month = obj->data;
	EXPECT_INT(month->year, 2000);
	EXPECT_INT(month->month, 3);
	char *s = Obj2str(obj);
	EXPECT_STR(s, "2000-03");
	memfree(s);
}

TEST_DECL(io)
{
	Month m = { 2001, 2 };
	Type_Month.print(&Type_Month, &m, OUT());
	EXPECT_OUT("\"2001-02\"");

	IO *tmp = io_tmpbuf(8);

	EXPECT(Type_Month.write(&Type_Month, &m, tmp) == 2);
	m.year = 0;
	m.month = 0;
	io_rewind(tmp);
	EXPECT(Type_Month.read(&Type_Month, &m, tmp) == 2);
	EXPECT(m.year == 2001);
	EXPECT(m.month == 2);

	io_close(tmp);
}

TEST_DECL(idx)
{
	EXPECT_TERM("Month[] mv = {"
		"Date(1,1,2010),"
		"Date(4,5,2011),"
		"Date(5,7,2011),"
		"}", "_Month0: {\"2010-01\", \"2011-05\", \"2011-07\"}", NULL);
	EXPECT_TERM("mv[Date(2,5,2011)]", "Month: \"2011-05\"", NULL);
	EXPECT_TERM("mv[Date(5,7,2012)]", "Month: NULL", NULL);
}

TEST_SUITE
{
	SetupStd();
	Month_setup();
	EXPECT(GetType("Month") == &Type_Month);

	TEST_RUN(conv);
	TEST_RUN(obj);
	TEST_RUN(io);
	TEST_RUN(idx);
	CLEANUP_TERM;
}
