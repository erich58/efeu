#include <EFEU/UnitTest.h>
#include <EFEU/object.h>
#include <EFEU/conv.h>
#include <math.h>

TEST_DECL(setup)
{
	SetProgName("test");
	SetApplPath(UnitTest_tmp(NULL));

	TEST_FILE("efmain.msg",
		"161	$!: could not convert $1 to $2.\n"
		"167	$!: operand of lvalue-cast is not an lvalue.\n"
		"169	$!: function argument $1 still in use.\n"
	);

	SetupStd();
}

TEST_DECL(convobj)
{
	EfiObj *obj;

	EXPECT(ConvObj(NULL, NULL) == NULL);

	obj = int2Obj(5);
	EXPECT(ConvObj(obj, NULL) == obj);
	obj = UnrefObj(obj);
	EXPECT(ConvObj(obj, &Type_int) == obj);
	obj = UnrefObj(obj);
	EXPECT(Obj2int(obj) == 5);

	obj = ConvObj(int2Obj(5), &Type_obj);

	UnitTest_stderr("stderr");
	EfiObj *undef = NewPtrObj(&Type_undef, mstrcpy("name"));
	EXPECT(ConvObj(obj, &Type_ptr) == NULL);
//	PRINT_CONTENT("stderr");
	EXPECT(ConvObj(undef, &Type_int) == NULL);
//	PRINT_CONTENT("stderr");
	EXPECT(LvalCast(obj, &Type_int) == NULL);
	PRINT_CONTENT("stderr");
	UnitTest_stderr(NULL);
//	EXPECT_CONTENT("stderr",
//		"MSG test: could not convert int to _Ptr_.\n"
//		"test: could not convert (_undef_) name to int.\n"
//		"test: operand of lvalue-cast is not an lvalue.\n"
//	);

	UnrefObj(undef);
	UnrefObj(obj);
}

TEST_DECL(conv)
{
	EfiConv conv;
	EXPECT(GetConv(NULL, NULL, NULL) == NULL);
	EXPECT_PTR(GetConv(&conv, &Type_int, &Type_int));
	int src = 42;
	int tg = 0;
	ConvData(&conv, &tg, &src);
	EXPECT(tg == 42);
	EXPECT(ConvDist(&Type_int, &Type_varint) == 0x1);
	EXPECT(ConvDist(&Type_ptr, &Type_str) == 0x4);
	EXPECT(ConvDist(&Type_varint, &Type_int) == 0x20);
	EXPECT(ConvDist(&Type_str, &Type_int) == 0x40);
}

TEST_DECL(LvalCast)
{
	char *s = NULL;
	EfiObj *o = LvalObj(&Lval_ptr, &Type_str, &s);
	EfiObj *c = LvalCast(RefObj(o), &Type_int);
	memfree(Obj_ident(c));
	EXPECT(c->type == &Type_int);
	int n = 42;
	CopyData(c->type, c->data, &n);
	EXPECT(Val_int(c->data) == 42);
	SyncLval(c);
	EXPECT_STR(s, "42");
	memfree(s);
	s = mstrcpy("256");
	UpdateLval(c);
	EXPECT(Val_int(c->data) == 256);
	PRINT_INT(o->refcount);
	PRINT_INT(c->refcount);

	UnrefObj(LvalCast(RefObj(o), &Type_int));

	UnitTest_stderr("stderr");
	UnrefObj(o);
	UnitTest_stderr(NULL);
	MATCH_CONTENT("stderr", "unlink");
	UnrefObj(c);
}

TEST_SUITE
{
	TEST_RUN(setup);
	TEST_RUN(convobj);
	TEST_RUN(conv);
	TEST_RUN(LvalCast);
}
