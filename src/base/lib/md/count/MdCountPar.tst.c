#include <EFEU/UnitTest.h>
#include <EFEU/mdmat.h>
#include <EFEU/mdcount.h>
#include <EFEU/EfiCount.h>
#include <EFEU/locale.h>
#include "utest/out.tst.h"
#include "utest/mstr.tst.h"

TEST_DECL(setup)
{
	setenv("LANG", "C", 1);
	SetProgName("ID");
	SetApplPath(UnitTest_tmp(NULL));
	SetupStd();
	SetupEfiCount();
	TEST_FILE("mdmat.msg",
		"306	$!: $1: undefined count method '$2'.\n"
	);
}

TEST_DECL(undefined_method)
{
	UnitTest_stderr("stderr");

	TEST_FORK
	{
		MdCountPar *par = MdCountPar_create();
		MdCountPar_init(par, int2Obj(1), "undefined");
		TEST_EXIT;
	}

	CHILD_EXIT(1);
	UnitTest_stderr(NULL);
	EXPECT_CONTENT("stderr",
		"ERR ID: int: undefined count method 'undefined'.\n"
	);
}

TEST_DECL(list_method)
{
	UnitTest_stdout("stdout");

	TEST_FORK
	{
		MdCountPar *par = MdCountPar_create();
		EfiObj *obj = strterm("struct DATA { int a; double b; } x");
		MdCountPar_init(par, EvalObj(obj, NULL), "?");
		TEST_EXIT;
	}

	CHILD_EXIT(0);
	UnitTest_stdout(NULL);
	EXPECT_CONTENT("stdout",
		"type=DATA\n"
		"record\t:*:count records:de:Datensätze zählen\n"
		"type=int\n"
		"a.record\t:*:count records:de:Datensätze zählen\n"
		"type=double\n"
		"b.record\t:*:count records:de:Datensätze zählen\n"
	);
}

TEST_DECL(info_method)
{
	UnitTest_stdout("stdout");

	TEST_FORK
	{
		MdCountPar *par = MdCountPar_create();
		MdCountPar_init(par, int2Obj(1), "record=?");
		TEST_EXIT;
	}

	CHILD_EXIT(0);
	UnitTest_stdout(NULL);
	EXPECT_CONTENT("stdout",
		"record\n"
		"\tcount records\n"
	);
}

TEST_DECL(simple)
{
	EfiObj *obj = ptr2Obj(NULL);
	MdCountPar *par = MdCountPar_create();
	EXPECT_MSTR(rd_ident(par), "MdCountPar[2, 0, 0]");;
	MdCountPar_init(par, obj, "record");
	par->md = md_ctab("Test", NULL, par, stdcount);
	EXPECT(MdCountPar_count(par) == 1);
	EXPECT(MdCountPar_count(par) == 1);
	md_print(io_refer(OUT()), par->md, "mode=std");
	EXPECT_NOUT(par->md,
		"##MDMAT   2.0\n"
		"##Title   Test\n"
		"##Type    int\n"
		"##Locale  C\n"
		"##Rows    \n"
		"##Columns \n"
		"1 .\n"
		". 2\n"
	);
	rd_deref(par);
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
	PRINT_SIZE(par->vtab.used);
	PRINT_SIZE(par->ctab.used);

//	par->md = md_ctab("Test", "time=Jahr", par, stdcount);
	par->md = md_ctab("Test", NULL, par, stdcount);

	Val_int(obj->data) = 42;
	EXPECT_INT(MdCountPar_count(par), 3);
	Val_int(obj->data) = 11;
	EXPECT_INT(MdCountPar_count(par), 3);

	md_print(io_refer(OUT()), par->md, "mode=std");
	EXPECT_NOUT(par->md,
		"##MDMAT   2.0\n"
		"##Title   Test\n"
		"##Type    int\n"
		"##Locale  C\n"
		"##Rows    \n"
		"##Columns \n"
		"1 .\n"
		". 6\n"
	);
	rd_deref(par);
	md_count_hook = NULL;
}

TEST_SUITE
{
	TEST_RUN(setup);
	TEST_RUN(undefined_method);
	TEST_RUN(simple);
	TEST_RUN(list_method);
	TEST_RUN(info_method);
	TEST_RUN(generic);
	CLEANUP_OUT;
	CLEANUP_MSTR;
}
