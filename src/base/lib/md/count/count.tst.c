#include <EFEU/UnitTest.h>
#include <EFEU/mdmat.h>
#include <EFEU/mdcount.h>
#include <EFEU/EfiCount.h>
#include <EFEU/locale.h>
#include "utest/out.tst.h"

TEST_DECL(setup)
{
	SetProgName("ID");
	SetLocale(LOC_DATE, "C");
	SetApplPath(UnitTest_tmp(NULL));
	SetupStd();
	SetupEfiCount();
	TEST_FILE("mdmat.msg",
		"303	$!: data cube not prepared for counting.\n"
	);
}

TEST_DECL(missing_args)
{
	EXPECT(md_count(NULL, NULL) == 0);
}

TEST_DECL(bad_cube)
{
	UnitTest_stderr("stderr");
	mdmat *md = md_create(&Type_int, NULL);

	TEST_FORK
	{
		md_count(md, NULL);
		TEST_EXIT;
	}

	CHILD_EXIT(1);
	UnitTest_stderr(NULL);
	rd_deref(md);
	EXPECT_CONTENT("stderr",
		"ERR ID: data cube not prepared for counting.\n"
	);
}

TEST_DECL(simple)
{
	EfiObj *obj = ptr2Obj(NULL);
	MdCountPar *par = MdCountPar_create();
	MdCountPar_init(par, obj, "record");
	mdmat *md = md_ctab("Test", NULL, par, stdcount);
	EXPECT(md_count(md, obj->data) == 1);
	EXPECT(md_count(md, obj->data) == 1);
	md_print(io_refer(OUT()), md, "mode=std");
	EXPECT_NOUT(md,
		"##MDMAT   2.0\n"
		"##Title   Test\n"
		"##Type    int\n"
		"##Locale  C\n"
		"##Rows    \n"
		"##Columns \n"
		"1 .\n"
		". 2\n"
	);
	rd_deref(md);
}

static void (test_count_hook) (MdCountList *clist, mdmat *md)
{
	md_count_add(clist, md->axis, md->data);
}

static const char *axis_info(mdaxis *x)
{
	static STRING_DECL(buf);
	buf.size = 0;

	if	(!x)	return NULL;

	UnitTest_puts(StrPool_get(x->sbuf, x->i_name), &buf);
	UnitTest_printf(&buf, "[%zd]", x->dim);
	const char *delim = "=";

	for (size_t n = 0; n < x->dim; n++)
	{
		UnitTest_puts(delim, &buf);
		UnitTest_puts(StrPool_get(x->sbuf, x->idx[n].i_name), &buf);
		delim = ",";
	}

	UnitTest_putc(0, &buf);
	return buf.data;
}

TEST_DECL(generic)
{
	md_count_hook = test_count_hook;
	EfiObj *obj = int2Obj(0);
	MdCountPar *par = MdCountPar_create();
	MdCountPar_range(par, "[name=Jahr]2000..2002");
	EXPECT_UINT(par->time_dim, 3);
	EXPECT_STR(axis_info(par->axis), "Jahr[3]=2000,2001,2002");
	EXPECT_UINT(par->axis->dim, 3);
	MdCountPar_init(par, obj, "record");


	mdmat *md = md_ctab("Test", NULL, par, stdcount);

	Val_int(obj->data) = 42;
	EXPECT(md_count(md, obj->data) == 1);
	Val_int(obj->data) = 11;
	EXPECT(md_count(md, obj->data) == 1);

	md_print(io_refer(OUT()), md, "mode=std");
	EXPECT_NOUT(md,
		"##MDMAT   2.0\n"
		"##Title   Test\n"
		"##Type    int\n"
		"##Locale  C\n"
		"##Rows    \n"
		"##Columns \n"
		"1 .\n"
		". 2\n"
	);
	rd_deref(md);
	md_count_hook = NULL;
}

TEST_SUITE
{
	TEST_RUN(setup);
	TEST_RUN(missing_args);
	TEST_RUN(bad_cube);
	TEST_RUN(simple);
	TEST_RUN(generic);
	CLEANUP_OUT;
}
