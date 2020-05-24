#include <EFEU/UnitTest.h>
#include <EFEU/object.h>
#include <EFEU/printobj.h>
#include <EFEU/Resource.h>
#include <EFEU/math.h>
#include <EFEU/CmdPar.h>
#include "utest/mstr.tst.h"
#include "utest/out.tst.h"
#include <math.h>

TEST_DECL(name)
{
	EfiObj *obj;
	void *data;

	obj = NewPtrObj(&Type_name, NULL);
	PrintData(OUT(), obj->type, obj->data);
	EXPECT_OUT("operator\"\"");
	data = (void *) "++";
	CopyData(obj->type, obj->data, &data);
	PrintData(OUT(), obj->type, obj->data);
	EXPECT_OUT("operator\"++\"");
	data = (void *) "abc";
	CopyData(obj->type, obj->data, &data);
	PrintData(OUT(), obj->type, obj->data);
	EXPECT_OUT("abc");
	obj = EvalObj(obj, NULL);
	EXPECT(obj->type == &Type_undef);
	EXPECT_MSTR(Obj2str(obj), "(_undef_) abc");

}

TEST_DECL(floatprec)
{
	SetFloatPrec("#4g");
	EXPECT_MSTR(Obj2str(strterm("12.5")), "12.50");
	SetFloatPrec("0f");
	EXPECT_MSTR(Obj2str(strterm("12.5")), "13");
	SetFloatPrec("#5");
}

TEST_SUITE
{
	SetupStd();
	SetupMath();
	TEST_RUN(name);
	TEST_RUN(floatprec);
	CLEANUP_MSTR;
	CLEANUP_OUT;
}
