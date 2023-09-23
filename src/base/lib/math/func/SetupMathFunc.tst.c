#include <EFEU/UnitTest.h>
#include <EFEU/object.h>
#include <Math/func.h>

TEST_SUITE
{
	SetupStd();
	SetupMathFunc();
	EXPECT_DBL(Obj2double(strterm("dnormal(0.)")), 0.3989422804014327);
	EXPECT_DBL(Obj2double(strterm("pnormal(0.)")), 0.5);
	EXPECT_DBL(Obj2double(strterm("qnormal(0.5)")), 0.);
	EXPECT_DBL(Obj2double(strterm("dlognormal(1.)")), 0.3989422804014327);
	EXPECT_DBL(Obj2double(strterm("plognormal(1.)")), 0.5);
	EXPECT_DBL(Obj2double(strterm("qlognormal(0.5)")), 1.);
}
