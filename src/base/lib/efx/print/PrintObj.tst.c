#include <EFEU/UnitTest.h>
#include <EFEU/object.h>
#include <EFEU/printobj.h>
#include <EFEU/utest/out.tst.h>
#include <EFEU/utest/mstr.tst.h>

#define	OBJ_PRINT(expr, value)	do { \
EfiObj *obj = EvalObj(expr, NULL); \
PrintObj(OUT(), obj); \
UnitTest_strcheck(__FILE__, __LINE__, "==", \
	utest_out(), value, # expr, # value); \
UnrefObj(obj); \
} while (0)


#define	PRINT_DATA(t, d, v) \
do { \
	PrintData(OUT(), t, d); \
	UnitTest_strcheck(__FILE__, __LINE__, "==", utest_out(), v, \
		utest_mstr(msprintf("print(%#s,%s)", t->name, # d)), # v); \
} while (0)

#define	SHOW_ANY(t, d, v) \
do { \
	ShowAny(OUT(), t, d); \
	UnitTest_strcheck(__FILE__, __LINE__, "==", utest_out(), v, \
		utest_mstr(msprintf("show(%#s,%s)", t->name, # d)), # v); \
} while (0)

TEST_DECL(PrintObj)
{
	EfiType *type;
	int data1[2] = { 11, 12 };
	int data2[2] = { 21, 22 };

	PrintObj(OUT(), NULL);
	EXPECT_OUT("void");

	type = &Type_int;
	PRINT_DATA(type, data1, "11");
	ShowVecData(OUT(), type, data2, 2);
	EXPECT_OUT("{21, 22}");

	type = NewType("Vector[2]");
	type->dim = 2;
	type->base = &Type_int;
	type->size = type->dim * Type_int.size;
	type->recl = type->dim * Type_int.recl;
	PRINT_DATA(type, data1, "{11, 12}");
	SHOW_ANY(type, data2, "{21, 22}");

	type = NewEnumType("Enum", sizeof(int));
	PRINT_DATA(type, data1, "11");
	SHOW_ANY(type, data2, "21");

	type = NewType("subclass(int)");
	type->base = &Type_int;
	type->size = Type_int.size;
	type->recl = Type_int.recl;
	PRINT_DATA(type, data1, "(subclass(int)) 11");
	SHOW_ANY(type, data2, "(subclass(int)) 21");
	ShowVecData(OUT(), type, data2, 2);
	EXPECT_OUT("{21, 22}");

	type = NewType("list(int, int)");
	type->base = &Type_int;
	type->list = NewEfiStruct(&Type_int, "a", 0);
	type->list->next = NewEfiStruct(&Type_int, "b", 0);
	type->list->next->offset = Type_int.size;
	type->size = 2 * Type_int.size;
	type->recl = 2 * Type_int.recl;
	PRINT_DATA(type, data1, "{11, 12}");
	SHOW_ANY(type, data2, "{21, 22}");

	type = NewType("Size4");
	type->size = 4;
	type->recl = 4;
	char data[4] = { 0x1, 0x2, 0x3, 0x4 };
	PRINT_DATA(type, data, "(Size4) 0x01020304");

	type = NewType("Size0");
	PRINT_DATA(type, data, "Size0()");
	SHOW_ANY(type, data, "Size0()");
	ShowVecData(OUT(), type, data, 2);
	EXPECT_OUT("{Size0(), Size0()}");
}

TEST_DECL(PrintFmtObj)
{
	EfiObj *obj = int2Obj(42);
	PrintFmtObj(OUT(), "|%d|", obj);
	EXPECT_OUT("|42|");
	UnrefObj(obj);
	PrintFmtObj(OUT(), NULL, NULL);
	EXPECT_OUT("");
}

TEST_SUITE
{
	SetupStd();
	TEST_RUN(PrintObj);
	TEST_RUN(PrintFmtObj);
	CLEANUP_OUT;
	CLEANUP_MSTR;
}
