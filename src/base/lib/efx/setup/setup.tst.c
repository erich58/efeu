#include <EFEU/UnitTest.h>
#include <EFEU/object.h>
#include <EFEU/printobj.h>
#include <EFEU/Resource.h>
#include <EFEU/math.h>
#include <EFEU/CmdPar.h>
#include <EFEU/utest/mstr.tst.h>
#include <EFEU/utest/out.tst.h>
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

TEST_SUITE
{
	SetupStd();
	SetupMath();
	TEST_RUN(name);
	CLEANUP_MSTR;
	CLEANUP_OUT;
}
