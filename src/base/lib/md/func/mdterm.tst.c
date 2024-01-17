#include <EFEU/UnitTest.h>
#include <EFEU/mdmat.h>
#include "utest/out.tst.h"

static void obj_test_func(EfiFunc *func, void *rval, void **arg)
{
	;
}

static EfiFuncDef fdef[] = {
	{ 0, &Type_obj, "obj_test_func (int x)", obj_test_func },
};

TEST_DECL(init)
{
	SetupMdMat();
	SetProgName("ID");
	SetApplPath(UnitTest_tmp(NULL));
	TEST_FILE("mdmat.msg",
		"91	$!: mdterm: function not defined.\n"
		"92	$!: mdterm: incompatible axis $1 - $2 (differnd sizes).\n"
		"93	$!: mdterm: function '$1' has unspecified return value.\n"
		"94	$!: mdterm: function '$1' is not defined.\n"
	);

	AddFuncDef(fdef, tabsize(fdef));
}

TEST_DECL(assign)
{
	mdmat *a = NULL;
	mdmat *b = NULL;

	EXPECT(md_assign(NULL, NULL, NULL) == NULL);

	EXPECT_PTR(a = md_scalar(int2Obj(1)));
	EXPECT(((int *)a->data)[0] == 1);
	UnitTest_stderr("stderr");
	EXPECT(md_assign(NULL, a, NULL) == a);
	UnitTest_stderr(NULL);
	EXPECT_CONTENT("stderr",
		"MSG ID: mdterm: function '' is not defined.\n");

	EXPECT(md_assign("++", a, NULL) == a);
	EXPECT(((int *)a->data)[0] == 2);

	EXPECT_PTR(b = md_scalar(int2Obj(5)));
	EXPECT(((int *)b->data)[0] == 5);
	EXPECT(md_assign("+=", a, b) == a);
	EXPECT(((int *)a->data)[0] == 7);

	rd_deref(a);
	rd_deref(b);
}

TEST_DECL(term)
{
	mdmat *a = NULL;
	mdmat *b = NULL;
	mdmat *x = NULL;
	EfiVirFunc *vfunc = NULL;

	EXPECT(md_term(NULL, NULL, NULL) == NULL);

	EXPECT_PTR(a = md_scalar(int2Obj(1)));
	EXPECT(((int *)a->data)[0] == 1);
	UnitTest_stderr("stderr");
	EXPECT(md_term(NULL, a, NULL) == a);
	UnitTest_stderr(NULL);
	EXPECT_CONTENT("stderr",
		"MSG ID: mdterm: function not defined.\n");

	EXPECT_PTR(vfunc = GetGlobalFunc("obj_test_func"));
	UnitTest_stderr("stderr");
	EXPECT(md_term(vfunc, a, NULL) == a);
	UnitTest_stderr(NULL);
	EXPECT_CONTENT("stderr",
		"MSG ID: mdterm: function 'obj_test_func' has unspecified return value.\n");
	rd_deref(vfunc);

	EXPECT_PTR(vfunc = GetGlobalFunc("-()"));
	EXPECT_PTR(x = md_term(vfunc, a, NULL));
	EXPECT(((int *)a->data)[0] == 1);
	EXPECT(((int *)x->data)[0] == -1);
	rd_deref(x);
	rd_deref(vfunc);

	EXPECT_PTR(b = md_scalar(int2Obj(5)));
	EXPECT_PTR(vfunc = GetGlobalFunc("+"));
	EXPECT_PTR(x = md_term(vfunc, a, b));
	EXPECT(((int *)a->data)[0] == 1);
	EXPECT(((int *)b->data)[0] == 5);
	EXPECT(((int *)x->data)[0] == 6);
	rd_deref(a);
	rd_deref(b);
	rd_deref(x);
	rd_deref(vfunc);
}

TEST_DECL(mixed)
{
	mdmat *a = NULL;
	mdmat *b = NULL;
	mdmat *x = NULL;
	EfiVirFunc *vfunc = NULL;

	EXPECT_PTR(a = md_create(&Type_int, "a=a1,a2,a3"));

	for (size_t n = 0; n < a->axis->dim; ++n)
		((int *) a->data)[n] = n + 1;

	md_print(io_refer(OUT()), a, "nohead");
	EXPECT_NOUT(a,
		"1 .\n"
		"a1 1\n"
		"a2 2\n"
		"a3 3\n"
	);

	EXPECT_PTR(b = md_create(&Type_int, "b=b1,b2 c=x,y"));

	for (size_t i = 0; i < 4; ++i)
		((int *) b->data)[i] = 100 * (i / 2 + 1) + 10 * (i % 2 + 1);

	md_print(io_refer(OUT()), b, "nohead");
	EXPECT_NOUT(b,
		"1 .\n"
		"b1.x 110\n"
		"b1.y 120\n"
		"b2.x 210\n"
		"b2.y 220\n"
	);

	EXPECT_PTR(vfunc = GetGlobalFunc("+"));

	UnitTest_stderr("stderr");
	EXPECT_PTR(x = md_term(vfunc, a, b));
	UnitTest_stderr(NULL);
	EXPECT_CONTENT("stderr",
		"MSG ID: mdterm: incompatible axis a - b (differnd sizes).\n");

	md_print(io_refer(OUT()), x, "nohead");
	EXPECT_NOUT(x,
		"1 .\n"
		"a1.x 111\n"
		"a1.y 121\n"
		"a2.x 212\n"
		"a2.y 222\n"
		"a3.x 0\n"
		"a3.y 0\n"
	);

	rd_deref(a);
	rd_deref(x);

	EXPECT_PTR(a = md_create(&Type_int, "a=a1,a2"));

	for (size_t n = 0; n < a->axis->dim; ++n)
		((int *) a->data)[n] = n + 1;

	md_print(io_refer(OUT()), a, "nohead");
	EXPECT_NOUT(a,
		"1 .\n"
		"a1 1\n"
		"a2 2\n"
	);

	EXPECT_PTR(x = md_term(vfunc, b, a));
	md_print(io_refer(OUT()), x, "nohead");
	EXPECT_NOUT(x,
		"1 .\n"
		"b1.x 111\n"
		"b1.y 121\n"
		"b2.x 212\n"
		"b2.y 222\n"
	);
	rd_deref(a);
	rd_deref(b);
	rd_deref(x);
	rd_deref(vfunc);
}

TEST_SUITE
{
	TEST_RUN(init);
	TEST_RUN(assign);
	TEST_RUN(term);
	TEST_RUN(mixed);
	CLEANUP_OUT;
}
