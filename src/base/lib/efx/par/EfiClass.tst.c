#include <EFEU/UnitTest.h>
#include <EFEU/EfiClass.h>
#include <EFEU/printobj.h>
#include "utest/out.tst.h"

static EfiObj *OBJ(int value)
{
	static EfiObj *obj = NULL;
	
	if	(obj)
		Val_int(obj->data) = value;
	else
		obj = int2Obj(value);

	return obj;
}

EfiClass EfiClass_dummy1 = EFI_CLASS(NULL,
	"dummy1", "desc", NULL, NULL,
	"$1$:\ntext"
);

TEST_DECL(dummy)
{
	EfiClass_info(OUT(), &EfiClass_dummy1);
	EXPECT_OUT("dummy1 desc\n\tdummy1:\n\ttext\n");
}

TEST_DECL(flag)
{
	EfiClassArg arg;
	EfiClass_info(OUT(), &EfiClass_flag);
	EXPECT_OUT(
		"flag[expr]=label desc\n"
		"\tflag expression\n"
	);

	EfiClass_flag.create(&arg, &Type_int, NULL, "name",
		EfiClass_flag.par);
	EXPECT_STR(arg.type->name, "_enum_name_1_");
	PrintType(OUT(), arg.type, 1);
	EXPECT_OUT("enum : 1 { name = 1, }");
	EXPECT(arg.par == NULL);
	EXPECT(arg.update(OBJ(0), arg.par) == 1);

	EfiClass_flag.create(&arg, &Type_int, "this == 5", "isfive",
		EfiClass_flag.par);
	EXPECT_STR(arg.type->name, "_enum_isfive_1_");
	PrintType(OUT(), arg.type, 1);
	EXPECT_OUT("enum : 1 { isfive = 1, }");
	EXPECT(arg.par != NULL);
	
	EXPECT(arg.update(OBJ(0), arg.par) == 0);
	EXPECT(arg.update(OBJ(5), arg.par) == 1);
	EXPECT(rd_deref(arg.par) == NULL);
}

TEST_DECL(cswitch)
{
	EfiClassArg arg;
	EfiClass_info(OUT(), &EfiClass_switch);
	EXPECT_OUT(
		"switch[fmt]=key label desc;...\n"
		"\tswitch expression\n"
	);

	EfiClass_switch.create(&arg, &Type_int, NULL,
		"\\3 three; 7 \"seven\"", EfiClass_switch.par);
	EXPECT_STR(arg.type->name, "_enum_seven_2_three_1_");
	PrintType(OUT(), arg.type, 1);
	EXPECT_OUT("enum : 1 { seven = 2, three = 1, }");
	EXPECT(arg.par != NULL);
	EXPECT(arg.update(OBJ(0), arg.par) == 0);
	EXPECT(arg.update(OBJ(3), arg.par) == 1);
	EXPECT(arg.update(OBJ(7), arg.par) == 2);
	EXPECT(arg.update(OBJ(10), arg.par) == 0);
	EXPECT(rd_deref(arg.par) == NULL);
}

TEST_DECL(ctest)
{
	EfiClassArg arg;
	EfiClass_info(OUT(), &EfiClass_test);
	EXPECT_OUT(
		"test[fmt]=key label desc;...\n"
		"\ttest expression\n"
	);

	EfiClass_test.create(&arg, &Type_int, "this % 2 == $1",
		"0 even; * odd", EfiClass_test.par);
	EXPECT_STR(arg.type->name, "_enum_even_1_odd_2_");
	PrintType(OUT(), arg.type, 1);
	EXPECT_OUT("enum : 1 { even = 1, odd = 2, }");
	EXPECT(arg.par != NULL);
	EXPECT(arg.update(OBJ(0), arg.par) == 1);
	EXPECT(arg.update(OBJ(1), arg.par) == 2);
	EXPECT(arg.update(OBJ(2), arg.par) == 1);
	EXPECT(rd_deref(arg.par) == NULL);
}

TEST_DECL(generic)
{
	EfiClassArg arg;
	EfiClass_info(OUT(), &EfiClass_generic);
	EXPECT_OUT(
		"generic[expr]=case label desc;...\n"
		"\tgeneric class construction\n"
	);

	EfiClass_generic.create(&arg, &Type_int, NULL,
		"@eval int lim = 10\n"
		"@case\n"
		"0 nul\n"
		"10 ten\n"
		"@end\n"
		"@test this > $1\n" 
		"5 gt5\n"
		"@end\n" 
		"@other rest\n",
		EfiClass_generic.par);
	EXPECT_STR(arg.type->name, "_enum_gt5_3_nul_1_rest_4_ten_2_");
	PrintType(OUT(), arg.type, 1);
	EXPECT_OUT("enum : 1 { gt5 = 3, nul = 1, rest = 4, ten = 2, }");
	EXPECT(arg.par != NULL);
	EXPECT(arg.update(OBJ(0), arg.par) == 1);
	EXPECT(arg.update(OBJ(1), arg.par) == 4);
	EXPECT(arg.update(OBJ(5), arg.par) == 4);
	EXPECT(arg.update(OBJ(8), arg.par) == 3);
	EXPECT(arg.update(OBJ(10), arg.par) == 2);
	EXPECT(arg.update(OBJ(20), arg.par) == 3);
	EXPECT(rd_deref(arg.par) == NULL);
}

static void visit(EfiParClass *class, void *data)
{
	io_printf(data, "%s: %s\n", class->epc_name, class->epc_label);
}

TEST_SUITE
{
	SetupStd();
	SetupEfiClass();
	EfiParWalk(NULL, &EfiPar_class, visit, OUT());
	EXPECT_OUT(
		"flag: flag expression\n"
		"generic: generic class construction\n"
		"switch: switch expression\n"
		"test: test expression\n"
	);

	TEST_RUN(dummy);
	TEST_RUN(flag);
	TEST_RUN(cswitch);
	TEST_RUN(ctest);
	TEST_RUN(generic);
	CLEANUP_OUT;
}
