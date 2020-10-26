#include <EFEU/UnitTest.h>
#include <EFEU/object.h>
#include <stdtype.h>
#include "utest/mstr.tst.h"

/* created with mksource <struct.ms> */

typedef struct {
	double d;		/* DBL */
	unsigned b1 : 24;	/* B1 */
	unsigned b2 : 8;	/* B2 */
	int i[2];		/* INT */
} TestType;

static void b1_get (unsigned *data, void *ptr)
{
	*data = ((TestType *) ptr)->b1;
}


static void b1_set (unsigned *data, void *ptr)
{
	((TestType *) ptr)->b1 = *data;
}

static void b2_get (unsigned *data, void *ptr)
{
	*data = ((TestType *) ptr)->b2;
}

static void b2_set (unsigned *data, void *ptr)
{
	((TestType *) ptr)->b2 = *data;
}

static EfiBFMember bftab[] = {
	{ "b1", "B1", b1_get, b1_set },
	{ "b2", "B2", b2_get, b2_set },
};

EfiType Type_Test = SIMPLE_TYPE("TestType", TestType, NULL, NULL);

TEST_SUITE
{
	SetupStd();

	EfiType *type = &Type_Test;
	AddType(type);
	AddTypeMember(type, offsetof(TestType, d), 0, "double", "d", "DBL");
	AddTypeMember(type, offsetof(TestType, i), 2, "int", "i", "INT");
	AddListConv(type, List2Struct, Struct2List);
	AddEfiBFMember(type->vtab, bftab, tabsize(bftab));

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
