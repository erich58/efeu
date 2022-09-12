#include <EFEU/UnitTest.h>
#include <EfiType/Period.h>
#include "utest/term.tst.h"
#include "utest/mstr.tst.h"

#define	eval(def)	Obj2str(strterm(def))

#define	EXPECT_EVAL(def, res)	EXPECT_MSTR(eval(def), res)

TEST_DECL(conv)
{
	EXPECT_EVAL("Period(NULL)", ":");
	EXPECT_EVAL("Period(\"2000\")", "2000-01-01:2000-12-31");
	EXPECT_EVAL("Period(\"2000:2010\")", "2000-01-01:2010-12-31");
	EXPECT_EVAL("Period(\"2000-02\")", "2000-02-01:2000-02-29");
	EXPECT_EVAL("Period(\"2000-02-10\")", "2000-02-10:2000-02-10");
	EXPECT_EVAL("Period(\"2010-12:\")", "2010-12-01:");
	EXPECT_EVAL("Period(\":2010-12\")", ":2010-12-31");
	EXPECT_EVAL("Period(\"2010\").beg", "2010-01-01");
	EXPECT_EVAL("Period(\"2010\").end", "2010-12-31");
	EXPECT_EVAL("Period(Date(12,1,2005))", "2005-01-12:2005-01-12");
	EXPECT_EVAL("Period(Date(12,1,2005),Date(4,2,2005))",
		"2005-01-12:2005-02-04");
}

TEST_DECL(Period_put)
{
	Period p;
	Period_set(&p, NULL);
	Period_put(&p, OUT(), 0);
	EXPECT_OUT(":");
	Period_put(&p, OUT(), 1);
	EXPECT_OUT("          :          ");
	Period_set(&p, ":2020");
	Period_put(&p, OUT(), 0);
	EXPECT_OUT(":2020-12-31");
	Period_put(&p, OUT(), 1);
	EXPECT_OUT("          :2020-12-31");
}

TEST_DECL(io)
{
	Period p;
	Period_set(&p, ":");
	EXPECT_INT(p.beg, 0);
	EXPECT_INT(p.end, 0);
	Type_Period.print(&Type_Period, &p, OUT());
	EXPECT_OUT("\":\"");

	Period_set(&p, "2020-06");
	PRINT_INT(p.beg);
	PRINT_INT(p.end);
	Type_Period.print(&Type_Period, &p, OUT());
	EXPECT_OUT("\"2020-06-01:2020-06-30\"");

	IO *tmp = io_tmpbuf(8);

	EXPECT(Type_Period.write(&Type_Period, &p, tmp) == 6);
	p.beg = 0;
	p.end = 0;
	io_rewind(tmp);
	EXPECT(Type_Period.read(&Type_Period, &p, tmp) == 6);
	Type_Period.print(&Type_Period, &p, OUT());
	EXPECT_OUT("\"2020-06-01:2020-06-30\"");

	io_close(tmp);
}

TEST_DECL(idx)
{
	PRINT_MSTR(eval("Period[] pv = "
		"{\"2010-01\", \"2010-02\",\"2010-03\"}"
	));
	EXPECT_TERM("pv[Date(5,2,2010)]",
		"Period: \"2010-02-01:2010-02-28\"", NULL);
	EXPECT_TERM("pv[Date(5,7,2012)]", "Period: \":\"", NULL);
}


TEST_SUITE
{
	SetupStd();
	Period_setup();
	EXPECT(GetType("Period") == &Type_Period);

	TEST_RUN(conv);
	TEST_RUN(Period_put);
	TEST_RUN(io);
	TEST_RUN(idx);
	CLEANUP_TERM;
	CLEANUP_MSTR;
}
