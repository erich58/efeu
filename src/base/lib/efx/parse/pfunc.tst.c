#include <EFEU/UnitTest.h>
#include <EFEU/parsedef.h>
#include <EFEU/printobj.h>
#include <EFEU/utest/in.tst.h>
#include <EFEU/utest/out.tst.h>

static const char *obj_to_str(EfiObj *obj)
{
	io_printf(OUT(), "%s: ", obj->type->name);
	PrintObj(OUT(), obj);
	UnrefObj(obj);
	return utest_out();
}

#define	EXPECT_OBJ(o,s)	UnitTest_strcheck(__FILE__, __LINE__, \
	"==", obj_to_str(o), s, # o, # s)
#define	MATCH_OBJ(o,s)	UnitTest_strcheck(__FILE__, __LINE__, \
	"~", obj_to_str(o), s, # o, # s)

TEST_DECL(p_const)
{
	EXPECT_OBJ(PFunc_bool(IN(""), ""), "bool: true");
	EXPECT_OBJ(PFunc_bool(IN(""), NULL), "bool: false");
	EXPECT_OBJ(PFunc_int(IN(""), (void *) 42), "int: 42");
	EXPECT_OBJ(PFunc_str(IN(""), "text"), "str: \"text\"");
	EXPECT_OBJ(PFunc_type(IN("int"), NULL), "Type_t: int");
}

TEST_SUITE
{
	SetupStd();
	TEST_RUN(p_const);
	CLEANUP_IN;
	CLEANUP_OUT;
}
