#include <EFEU/UnitTest.h>
#include "mdcount.h"
#include <EFEU/printobj.h>
#include "EfiCount.h"
#include <EFEU/preproc.h>
#include <EFEU/locale.h>
#include <EFEU/utest/out.tst.h>

static EfiType *str_type(const char *def)
{
	IO *io = io_cmdpreproc(io_cstr(def));
	EfiType *type = Parse_type(io, NULL);
	io_close(io);
	return type;
}

static const char *obj_class(char *name, const char *def)
{
	MdCountPar *par = MdCountPar_create();
	EfiObj *obj = ConstObj(str_type(def), NULL);
	MdCntObjClass(par, obj, name, "desc");
	MdShowClass(OUT(), par, "*");
	rd_deref(par);
	return utest_out();
}

TEST_DECL(classify)
{
	MdCountPar *par = MdCountPar_create();
	/* ACHTUNG: Komponente a kann nicht verändert werden */
	TEST_EXPR(streval("struct { enum {} a; enum {x, y, z} b} x;"));
	EfiObj *obj = GetVar(NULL, "x", NULL);
	MdCountPar_init(par, obj, "record");
	MdCntObjClass(par, RefObj(obj), "x", "desc");

	MdClass *ca, *cb;

	EXPECT_PTR(ca = MdClass_get(par, "x.a"));
	EXPECT_PTR(cb = MdClass_get(par, "x.b"));

	PrintObj(OUT(), obj);
	PRINT_OUT();
	EXPECT_INT(ca->classify(ca, NULL), 1);
	EXPECT_INT(cb->classify(cb, NULL), 0);

	TEST_EXPR(streval("x.b=2;"));
	PrintObj(OUT(), obj);
	PRINT_OUT();
	EXPECT_INT(cb->classify(cb, NULL), 2);

	/*

	*/

	rd_deref(par);
}

TEST_SUITE
{
	SetupStd();
	SetupEfiCount();
	EXPECT_STR(obj_class("a", "int"), "");

	EXPECT_STR(obj_class("b", "enum {}"),
		"b[1]\tdesc\n"
		"\tb\tdesc\n"
	);

	EXPECT_STR(obj_class("c", "enum {a/*A*/, b/*B*/, c/*C*/}"),
		"c[3]\tdesc\n"
		"\ta\tA\n"
		"\tb\tB\n"
		"\tc\tC\n"
	);

	EXPECT_STR(obj_class("d", "struct { int a; enum {x, y} b, c}"),
		"d.b[2]\t\n"
		"\tx\t\n"
		"\ty\t\n"
		"d.c[2]\t\n"
		"\tx\t\n"
		"\ty\t\n"
	);

	PRINT_LABEL("Vector types are ignored");
	EXPECT_STR(obj_class("x", "enum { a }[5]"), "");

	TEST_RUN(classify);
	CLEANUP_OUT;
}
