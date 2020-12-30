#include <EFEU/UnitTest.h>
#include <EFEU/EfiClass.h>
#include <EFEU/printobj.h>
#include <EFEU/stdtype.h>
#include "utest/out.tst.h"

static int print_Test (const EfiType *type, const void *data, IO *io)
{
	return io_printf(io, "%d", Val_int(data));
}

EfiType Type_Test = SIMPLE_TYPE("Test", int, &Type_int, print_Test);

static EfiObj *OBJ(int value)
{
	static EfiObj *obj = NULL;
	
	if	(!obj)
		obj = NewObj(&Type_Test, NULL);

	Val_int(obj->data) = value;
	return obj;
}

TEST_SUITE
{
	SetupStd();
	AddType(&Type_Test);
	SetupEfiClass();
	PrintType(OUT(), &Type_Test, 1);
	EXPECT_OUT("Test");
	ShowVarTab(OUT(), NULL, Type_Test.vtab);
	EXPECT_OUT("Test = {\n}");

	EXPECT(MakeEfiClass(&Type_Test, "numklass",
		"switch=5:isfive;*:isnotfive"));
	ShowVarTab(OUT(), NULL, Type_Test.vtab);
	EXPECT_OUT("Test = {\n"
		"\tnumklass\n"
		"}");
	EXPECT(Obj2int(GetVar(Type_Test.vtab, "numklass", OBJ(5))) == 1);
	EXPECT(Obj2int(GetVar(Type_Test.vtab, "numklass", OBJ(1))) == 2);

	EXPECT(MakeEfiClass(&Type_Test, "anyname", "undefined") == 0);

	UnitTest_stderr("stderr");
	EXPECT(MakeEfiClass(&Type_Test, "anyname", NULL) == 0);
	UnitTest_stderr(NULL);
	EXPECT_CONTENT("stderr",
		"flag         "
		"generic      "
		"switch       "
		"test\n");

	UnitTest_stderr("stderr");
	EXPECT(MakeEfiClass(&Type_Test, "anyname", " \t") == 0);
	UnitTest_stderr(NULL);
	EXPECT_CONTENT("stderr",
		"flag         "
		"generic      "
		"switch       "
		"test\n");

	UnitTest_stderr("stderr");
	EXPECT(MakeEfiClass(&Type_Test, "anyname", "flag=?") == 0);
	UnitTest_stderr(NULL);
	EXPECT_CONTENT("stderr",
		"flag[expr]=label desc\n"
		"\tflag expression\n"
	);

	UnitTest_stderr("stderr");
	EXPECT(MakeEfiClass(&Type_Test, "anyname", "?") == 0);
	UnitTest_stderr(NULL);
	EXPECT_CONTENT("stderr",
		"flag\tflag expression\n"
		"generic\tgeneric class construction\n"
		"switch\tswitch expression\n"
		"test\ttest expression\n"
	);

	UnitTest_stderr("stderr");
	EXPECT(MakeEfiClass(&Type_Test, "anyname", "??") == 0);
	UnitTest_stderr(NULL);
	EXPECT_CONTENT("stderr",
		"flag[expr]=label desc\n"
		"\tflag expression\n"
		"generic[expr]=case label desc;...\n"
		"\tgeneric class construction\n"
		"switch[fmt]=key label desc;...\n"
		"\tswitch expression\n"
		"test[fmt]=key label desc;...\n"
		"\ttest expression\n"
	);

	CLEANUP_OUT;
}
