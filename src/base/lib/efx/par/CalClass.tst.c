#include <EFEU/UnitTest.h>
#include <EFEU/EfiClass.h>
#include <EFEU/printobj.h>
#include <EFEU/calendar.h>
#include <EFEU/utest/out.tst.h>

static EfiObj *DATE(int d, int m, int j)
{
	static EfiObj *obj = NULL;
	
	if	(!obj)
		obj = NewObj(&Type_Date, NULL);

	Val_Date(obj->data) = CalendarIndex(d, m, j);
	return obj;
}


static int get_cal(EfiObj *obj)
{
	return Val_Date(obj->data);
}

TEST_SUITE
{
	SetupStd();
	EfiClassArg arg;

	CalClass(&arg, &Type_Date, "%d.%m.", "2020-01-05:2020-01-10", get_cal);
	PrintType(OUT(), arg.type, 1);
	EXPECT_OUT("enum : 1 { "
		"\"05.01.\" = 1, \"06.01.\" = 2, \"07.01.\" = 3, "
		"\"08.01.\" = 4, \"09.01.\" = 5, \"10.01.\" = 6, }"
	);
	EXPECT(arg.par != NULL);
	EXPECT(arg.update(DATE(1, 1, 2020), arg.par) == 0);
	EXPECT(arg.update(DATE(5, 1, 2020), arg.par) == 1);
	EXPECT(arg.update(DATE(10, 1, 2020), arg.par) == 6);
	EXPECT(arg.update(DATE(11, 1, 2020), arg.par) == 0);
	EXPECT(rd_deref(arg.par) == NULL);

	CalClass(&arg, &Type_int, "%d.%m.", "2020-01-05", get_cal);
	PrintType(OUT(), arg.type, 1);
	EXPECT_OUT("enum : 1 { \"05.01.\" = 1, }");
	EXPECT(rd_deref(arg.par) == NULL);

	CLEANUP_OUT;
}
