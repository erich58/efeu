#include <EFEU/UnitTest.h>
#include <EFEU/mktype.h>
#include <stdtype.h>
#include "utest/mstr.tst.h"

typedef struct {
	double d;		/* D */
	uint32_t b1 : 24;	/* b1 */
	uint32_t b2 : 8;	/* b2 */
	int i[2];		/* INT */
} TestType;

static void b1_get (void *data, void *ptr)
{
	*((int *) data) = ((TestType *) ptr)->b1;
}


static void b1_set (void *data, void *ptr)
{
	((TestType *) ptr)->b1 = *((int *) data);
}

static EfiBitfield Test_b1 = {
	"int", "b1", 24, b1_get, b1_set, "b1"
};

static void b2_get (void *data, void *ptr)
{
	*((int *) data) = ((TestType *) ptr)->b2;
}

static void b2_set (void *data, void *ptr)
{
	((TestType *) ptr)->b2 = *((int *) data);
}

static EfiBitfield Test_b2 = {
	"int", "b2", 8, b2_get, b2_set, "b2"
};

EfiType Type_Test = SIMPLE_TYPE("TestType", TestType, NULL, NULL);

TEST_SUITE
{
	SetupStd();

	EfiType *type;
	EfiStruct *st_var;
	size_t offset;

	type = &Type_Test;
	AddType(type);
	st_var = type->list;
	st_var = EfiType_stdvar(type, st_var, offsetof(TestType, d),
		0, "double", "d", "DBL");
	offset = st_var->offset + st_var->type->size;
	st_var = EfiType_bitfield(type, st_var, offset, &Test_b1);
	st_var = EfiType_bitfield(type, NULL, offset, &Test_b2);
	st_var = EfiType_stdvar(type, st_var, offsetof(TestType, i),
		2, "int", "i", "INT");

	PushVarTab(NULL, NULL);
	TestType data = { 5., 100, 10, { 1, 2 } };
	EfiVarDef var_data = { "data", &Type_Test, &data, "test data" };
	AddVarDef(NULL, &var_data, 1);
	EXPECT(Obj2double(strterm("data.d")) == 5.);
	EXPECT(Obj2int(strterm("data.b1")) == 100);
	EXPECT(Obj2int(strterm("data.b2")) == 10);
	EXPECT(Obj2int(strterm("data.i[0]")) == 1);
	EXPECT(Obj2int(strterm("data.i[1]")) == 2);
	streval("data.b1 = 5;");
	EXPECT(data.b1 == 5);
	streval("List_t l = { data.b1, data.b2 };");
	streval("l.obj++;");
	EXPECT(data.b1 == 6);
	streval("whatis(data.b1)");
	PopVarTab();

	CLEANUP_MSTR;
}
