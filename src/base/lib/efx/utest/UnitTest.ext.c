/*	Test-Framework für Skripts
*/

#include <EFEU/UnitTest.h>
#include <EFEU/pconfig.h>
#include <EFEU/preproc.h>
#include <EFEU/efutil.h>
#include <EFEU/printobj.h>

void UnitTest_setup(void);

static EfiVarTab *UnitTestVar = NULL;

static EfiVarDef vardef[] = {
	{ "UnitTest",	&Type_vtab, &UnitTestVar,
		"UnitTest-Funktionen\n" },
};

static StrBuf *do_print(void *fmt, void *list)
{
	StrBuf *sb = sb_acquire();
	IO *io = io_strbuf(sb);
	PrintFmtList(io, Val_str(fmt), Val_list(list));
	io_close(io);
	return sb;
}

static void f_info (EfiFunc *func, void *rval, void **arg)
{
	StrBuf *sb = do_print(arg[1], arg[2]);
	UnitTest_info(Val_str(arg[0]), 0, "%s", sb_nul(sb));
	sb_release(sb);
}

static void f_result (EfiFunc *func, void *rval, void **arg)
{
	StrBuf *sb = do_print(arg[2], arg[3]);
	Val_bool(rval) = UnitTest_result(Val_str(arg[0]), 0,
		Val_bool(arg[1]), "%s", sb_nul(sb));
	sb_release(sb);
}

static void f_strinfo (EfiFunc *func, void *rval, void **arg)
{
	UnitTest_strinfo(Val_str(arg[0]), 0,
		Val_str(arg[1]), Val_str(arg[2]));
}

static void f_strcheck (EfiFunc *func, void *rval, void **arg)
{
	Val_bool(rval) = UnitTest_strcheck(Val_str(arg[0]), 0,
		Val_str(arg[1]), Val_str(arg[2]), Val_str(arg[3]),
		Val_str(arg[4]), Val_str(arg[5]));
}

static void f_dcmp (EfiFunc *func, void *rval, void **arg)
{
	Val_bool(rval) = UnitTest_dcmp(Val_double(arg[0]), Val_double(arg[1]));
}

static void f_create (EfiFunc *func, void *rval, void **arg)
{
	Val_bool(rval) = UnitTest_create(Val_str(arg[0]), 0,
		Val_str(arg[1]), Val_str(arg[2]), Val_str(arg[3]));
}

static void f_assert (EfiFunc *func, void *rval, void **arg)
{
	Val_bool(rval) = UnitTest_assert(Val_bool(arg[0]));
}

static void f_tpl (EfiFunc *func, void *rval, void **arg)
{
	Val_str(rval) = mstrcpy(UnitTest_tpl(Val_str(arg[0])));
}

static void f_tmp (EfiFunc *func, void *rval, void **arg)
{
	Val_str(rval) = mstrcpy(UnitTest_tmpname(Val_str(arg[0])));
}

static void f_mkdir (EfiFunc *func, void *rval, void **arg)
{
	Val_str(rval) = mstrcpy(UnitTest_mkdir(Val_str(arg[0])));
}

static void f_mkfile (EfiFunc *func, void *rval, void **arg)
{
	Val_str(rval) = mstrcpy(UnitTest_mkfile(Val_str(arg[0]), Val_str(arg[1])));
}

static void f_content (EfiFunc *func, void *rval, void **arg)
{
	Val_str(rval) = mstrcpy(UnitTest_content(Val_str(arg[0])));
}

static void f_tmpclean (EfiFunc *func, void *rval, void **arg)
{
	UnitTest_tmpclean();
}

static void f_stdin (EfiFunc *func, void *rval, void **arg)
{
	Val_bool(rval) = UnitTest_stdin(Val_str(arg[0]), Val_str(arg[1]));
}

static void f_stdout (EfiFunc *func, void *rval, void **arg)
{
	Val_bool(rval) = UnitTest_stdout(Val_str(arg[0]));
}

static void f_stderr (EfiFunc *func, void *rval, void **arg)
{
	Val_bool(rval) = UnitTest_stderr(Val_str(arg[0]));
}

static void f_fork (EfiFunc *func, void *rval, void **arg)
{
	Val_bool(rval) = UnitTest_fork(Val_str(arg[0]), 0);
}

static void f_exit (EfiFunc *func, void *rval, void **arg)
{
	UnitTest_exit();
}

static void f_wait (EfiFunc *func, void *rval, void **arg)
{
	Val_int(rval) = UnitTest_wait();
}

static EfiFuncDef fdef_func[] = {
	{ 0, &Type_void, "UnitTest::info(str id, str fmt, ...)", f_info },
	{ 0, &Type_void, "UnitTest::strinfo(str id, str a, str x)", f_strinfo },
	{ 0, &Type_bool, "UnitTest::result(str id, bool res, str fmt, ...)",
		f_result },
	{ 0, &Type_bool, "UnitTest::strcheck(str id, str op, "
		"str s1, str s2, str x1, str x2)", f_strcheck },
	{ 0, &Type_bool, "UnitTest::dcmp(double a, double b)", f_dcmp },
	{ 0, &Type_bool, "UnitTest::create(str id, "
		"str name, str content, str expr)", f_create },
	{ 0, &Type_bool, "UnitTest::assert(bool stat)", f_assert },
	{ 0, &Type_str, "UnitTest::tpl(str tpl = NULL)", f_tpl },
	{ 0, &Type_str, "UnitTest::tmp(str name = NULL)", f_tmp },
	{ 0, &Type_str, "UnitTest::mkdir(str name)", f_mkdir },
	{ 0, &Type_str, "UnitTest::mkfile(str name, str content = NULL)", f_mkfile },
	{ 0, &Type_str, "UnitTest::content(str name = NULL)", f_content },
	{ 0, &Type_void, "UnitTest::tmpclean()", f_tmpclean },
	{ 0, &Type_bool, "UnitTest::stdin(str name, "
		"str content = NULL)", f_stdin },
	{ 0, &Type_bool, "UnitTest::stdout(str name)", f_stdout },
	{ 0, &Type_bool, "UnitTest::stderr(str name)", f_stderr },

	{ 0, &Type_bool, "UnitTest::fork(str id)", f_fork },
	{ 0, &Type_void, "UnitTest::exit()", f_exit },
	{ 0, &Type_int, "UnitTest::wait()", f_wait },
};

static PPMacDef macdef[] = {
	{ "PRINT_LABEL(x) UnitTest::info(__ID__, \"%s\", x)", NULL },
	{ "PRINT_STR(x) UnitTest::strinfo(__ID__, x, # x)", NULL },
	{ "PRINT(x) UnitTest::info(__ID__,"
		"\"%s -> %s\", # x, (str)(x))", NULL },
	{ "EXPECT(x) UnitTest::result(__ID__, (x), \"%s\", # x)", NULL },
	{ "ASSERT(x) UnitTest::assert(EXPECT(x))", NULL },
	{ "EXPECT_DBL(a, b) UnitTest::result(__ID__,"
		"UnitTest::dcmp(a, b), \"%s ~ %s\", # a, # b)", NULL },
	{ "EXPECT_STR(a, b) UnitTest::strcheck(__ID__,"
		"\"==\", a, b, # a, # b)", NULL },
	{ "EXPECT_STRCMP(a, op, b) UnitTest::strcheck(__ID__,"
		"op, a, b, # a, # b)", NULL },
	{ "MATCH_STR(a, b) UnitTest::strcheck(__ID__,"
		"\"~\", a, b, # a, # b)", NULL },
	{ "TEST_DIR(name) UnitTest::result(__ID__,"
		"UnitTest::mkdir(name), \"mkdir(%s)\", # name)", NULL },
	{ "TEST_FILE(name, cont) UnitTest::create(__ID__,"
		"name, cont, # cont)", NULL },
	{ "PRINT_CONTENT(name) UnitTest::strinfo(__ID__,"
		"UnitTest::content(name), sprintf(\"content(%s)\", # name))", NULL },
	{ "TEST_FORK if (UnitTest::fork(__ID__))", NULL },
	{ "TEST_EXIT UnitTest::wait()", NULL },
	{ "CHILD_SIGNALED(n) UnitTest::result(__ID__,"
		"UnitTest::wait() == -n, \"child signaled %d\", n)", NULL },
	{ "CHILD_EXIT(n) UnitTest::result(__ID__,"
		"UnitTest::wait() == n, \"child exit %d\", n)", NULL },
};

TEST_SUITE
{
}

void UnitTest_setup(void)
{
	static int init_done = 0;

	if	(init_done)
		return;

	init_done = 1;
	SetupStd();
	UnitTestVar = VarTab("UnitTest", 32);
	AddVarDef(GlobalVar, vardef, tabsize(vardef));
	PushVarTab(RefVarTab(UnitTestVar), NULL);
	AddFuncDef(fdef_func, tabsize(fdef_func));
	PopVarTab();
	AddMacDef(macdef, tabsize(macdef));
}
