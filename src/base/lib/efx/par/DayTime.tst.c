#include <EFEU/UnitTest.h>
#include <EFEU/EfiClass.h>
#include <EFEU/printobj.h>
#include <EFEU/calendar.h>
#include <EFEU/utest/out.tst.h>

static EfiObj *TIME(const char *def)
{
	static EfiObj *obj = NULL;
	
	if	(!obj)
		obj = NewObj(&Type_int, NULL);

	Val_int(obj->data) = str2sec(def, NULL);
	return obj;
}

static int get_sec (const EfiObj *obj)
{
	return Val_int(obj->data);
}

TEST_SUITE
{
	SetupStd();
	EfiClassArg arg;

	DayTime(&arg, &Type_int, NULL, "6h", get_sec);
	PrintType(OUT(), arg.type, 1);
	EXPECT_OUT("enum : 1 { \"00\" = 1, \"06\" = 2, "
		"\"12\" = 3, \"18\" = 4, }");
	EXPECT(arg.par != NULL);
	EXPECT(arg.update(TIME("5h30m"), arg.par) == 1);
	EXPECT(arg.update(TIME("12h"), arg.par) == 3);
	EXPECT(rd_deref(arg.par) == NULL);

	DayTime(&arg, &Type_int, "s%s-%x", "2s,10s", get_sec);
	PrintType(OUT(), arg.type, 1);
	EXPECT_OUT("enum : 1 { \"s00-x\" = 1, \"s02-x\" = 2, "
		"\"s04-x\" = 3, \"s06-x\" = 4, \"s08-x\" = 5, }");
	EXPECT(rd_deref(arg.par) == NULL);

	DayTime(&arg, &Type_int, NULL, "2s,10s", get_sec);
	PrintType(OUT(), arg.type, 1);
	EXPECT_OUT("enum : 1 { \"00:00:00\" = 1, \"00:00:02\" = 2, "
		"\"00:00:04\" = 3, \"00:00:06\" = 4, \"00:00:08\" = 5, }");
	EXPECT(rd_deref(arg.par) == NULL);

	CLEANUP_OUT;
}
