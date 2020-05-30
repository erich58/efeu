/*	Test-Framework für Skripts
*/

#include <EFEU/UnitTest.h>
#include <EFEU/pconfig.h>
#include <EFEU/preproc.h>
#include <EFEU/efutil.h>
#include <EFEU/printobj.h>
#include <EFEU/parsedef.h>
#include <EFEU/cmdeval.h>

void UnitTest_setup(void);
int UnitTest_iotest(const char *fn, int lno, const EfiObj *obj,
	const char *repr, const char *expr);

static EfiVarTab *UnitTestVar = NULL;
static STRBUF(UnitTestBuffer, 256);

static EfiVarDef vardef[] = {
	{ "UnitTest",	&Type_vtab, &UnitTestVar,
		"UnitTest-Funktionen\n" },
};

static EfiVarDef uvar[] = {
	{ "name", &Type_str, &UnitTest_name,
		"Basisname des Tests\n" },
	{ "child", &Type_int, &UnitTest_child,
		"Kindprozess\n" },
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
	UnitTest_info(Val_str(arg[0]), 0, "INFO", "%s", sb_nul(sb));
	sb_release(sb);
}

static void f_expr (EfiFunc *func, void *rval, void **arg)
{
	UnitTest_expr(Val_str(arg[0]), 0, Val_str(arg[1]));
}

static void f_result (EfiFunc *func, void *rval, void **arg)
{
	StrBuf *sb = do_print(arg[2], arg[3]);
	Val_bool(rval) = UnitTest_result(Val_str(arg[0]), 0,
		Val_bool(arg[1]), "%s", sb_nul(sb));
	sb_release(sb);
}

static char *is_expr(char *p)
{
	IO *io = io_string(p, NULL);
	EfiObj *obj = Parse_term(io, EOF);
	io_close(io);

	if	(obj && obj->type == &Type_str)
		p = NULL;

	UnrefObj(obj);
	return p;
}

static void f_strinfo (EfiFunc *func, void *rval, void **arg)
{
	UnitTest_strinfo(Val_str(arg[0]), 0, "INFO",
		Val_str(arg[1]), is_expr(Val_str(arg[2])));
}

static void f_strcheck (EfiFunc *func, void *rval, void **arg)
{
	Val_bool(rval) = UnitTest_strcheck(Val_str(arg[0]), 0,
		Val_str(arg[1]), Val_str(arg[2]), Val_str(arg[3]),
		Val_str(arg[4]), is_expr(Val_str(arg[5])));
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

static void f_repr (EfiFunc *func, void *rval, void **arg)
{
	StrBuf *sb = sb_acquire();
	IO *io = io_strbuf(sb);
	ShowObj(io, arg[0]);
	Val_str(rval) = sb_cpyrelease(sb);
}

static void f_out_io (EfiFunc *func, void *rval, void **arg)
{
	Val_io(rval) = io_strbuf(&UnitTestBuffer);
}

static void f_out_data (EfiFunc *func, void *rval, void **arg)
{
	Val_str(rval) = mstrcpy(sb_nul(&UnitTestBuffer));
	sb_trunc(&UnitTestBuffer);
}

static void f_iotest (EfiFunc *func, void *rval, void **arg)
{
	const EfiObj *obj = arg[1];

	if	(!obj)
	{
		Val_bool(rval) = 1;
		return;
	}

	StrBuf *sb = sb_acquire();
	IO *io = io_strbuf(sb);
	EfiObj *tg;
	int r = 1;

	const char *id = Val_str(arg[0]);
	const char *value = Val_str(arg[2]);
	const char *expr = Val_str(arg[3]);

	/*
	io_xprintf(io, "typeof(%s) -> %s (%s)",
		expr, obj->type->name, obj->type->cname); 
	UnitTest_info(id, 0, "# %s", sb_nul(sb));

	io_rewind(io);
	*/

	PrintData(io, obj->type, obj->data);
	r = (mstrcmp(sb_nul(sb), value) == 0);
	UnitTest_result(id, 0, r, "%s -> %s", expr,
		UnitTest_qstr(value));

	tg = ConstObj(obj->type, obj->data);
	io_rewind(io);
	PrintData(io, tg->type, tg->data); 
	r &= UnitTest_strcheck(id, 0, "==", sb_nul(sb), value,
		"copy", UnitTest_qstr(value));
	UnrefObj(tg);

	if	(r && obj->type->read && obj->type->write)
	{
		tg = NewObj(obj->type, NULL);
		io_rewind(io);
		WriteData(obj->type, obj->data, io); 
		io_rewind(io);
		ReadData(tg->type, tg->data, io); 
		io_rewind(io);
		PrintData(io, tg->type, tg->data); 
		r &= UnitTest_strcheck(id, 0, "==", sb_nul(sb), value, 
			"out-in", UnitTest_qstr(value));
		UnrefObj(tg);
	}

	sb_release(sb);
	Val_bool(rval) = r;
}


static EfiObj *PFunc_TestBlock (IO *io, void *data)
{
	char *name;
	EfiObj *block;
	EfiObjList *list;
	int rval = 0;

	char *id = rd_ident(io);

	if	(!Parse_fmt(io, "(N L) C", &name, &list, &block))
	{
		memfree(id);
		return NULL;
	}

	rval = Obj2int(ReduceObjList(&list));
	DelObjList(list);
	UnitTest_info(id, 0, "# Test: %s", name);
	memfree(id);
	memfree(name);

	if	(UnitTest_fork(id, 0))
	{
		UnrefEval(block);
		UnitTest_exit();
	}

	id = rd_ident(io);
	int estat = UnitTest_wait();

	if	(!UnitTest_result(id, 0, estat == rval, "child %s %d",
		rval < 0 ? "signaled" :"exit", abs(rval)))
	{
		UnitTest_info(id, 0, "BECAUSE", "child %s %d",
			estat < 0 ? "signaled" :"exit", abs(estat));
	}

	memfree(id);
	return NULL;
}


static EfiFuncDef fdef_func[] = {
	{ 0, &Type_void, "UnitTest::info(str id, str fmt, ...)", f_info },
	{ 0, &Type_void, "UnitTest::expr(str id, str expr)", f_expr },
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
	{ 0, &Type_bool, "UnitTest::iotest(str id, ., str repr, str expr)",
		f_iotest },
	{ 0, &Type_str, "UnitTest::content(str name = NULL)", f_content },
	{ 0, &Type_void, "UnitTest::tmpclean()", f_tmpclean },
	{ 0, &Type_bool, "UnitTest::stdin(str name = NULL, "
		"str content = NULL)", f_stdin },
	{ 0, &Type_bool, "UnitTest::stdout(str name = NULL)", f_stdout },
	{ 0, &Type_bool, "UnitTest::stderr(str name = NULL)", f_stderr },

	{ 0, &Type_bool, "UnitTest::fork(str id)", f_fork },
	{ 0, &Type_void, "UnitTest::exit()", f_exit },
	{ 0, &Type_int, "UnitTest::wait()", f_wait },
	{ 0, &Type_str, "UnitTest::repr(.)", f_repr },
	{ 0, &Type_io, "UnitTest::out_io()", f_out_io },
	{ 0, &Type_str, "UnitTest::out_data()", f_out_data },
};

static PPMacDef macdef[] = {
	{ "TEST_EXPR(__x) UnitTest::expr(__ID__, # __x), (__x)", NULL },
	{ "PRINT_LABEL(__x) UnitTest::info(__ID__, \"%s\", __x)", NULL },
	{ "PRINT_STR(__x) UnitTest::strinfo(__ID__, __x, # __x)", NULL },
	{ "PRINT(__x) UnitTest::info(__ID__,"
		"\"%s -> %s\", # __x, UnitTest::repr(__x))", NULL },
	{ "EXPECT(__x) UnitTest::result(__ID__, (__x), \"%s\", # __x)", NULL },
	{ "ASSERT(__x) UnitTest::assert(EXPECT(__x))", NULL },
	{ "EXPECT_DBL(__a, __b) UnitTest::result(__ID__,"
		"UnitTest::dcmp(__a, __b), \"%s ~ %s\", # __a, # __b)", NULL },
	{ "EXPECT_STR(__a, __b) UnitTest::strcheck(__ID__,"
		"\"==\", __a, __b, # __a, # __b)", NULL },
	{ "EXPECT_STRCMP(__a, __op, __b) UnitTest::strcheck(__ID__,"
		"__op, __a, __b, # __a, # __b)", NULL },
	{ "EXPECT_IOTEST(__a, __b) UnitTest::iotest(__ID__,"
		"__a, __b, # __a)", NULL },
	{ "MATCH_STR(__a, __b) UnitTest::strcheck(__ID__,"
		"\"~\", __a, __b, # __a, # __b)", NULL },
	{ "TEST_DIR(__n) UnitTest::result(__ID__,"
		"UnitTest::mkdir(__n), \"mkdir(%s)\", # __n)", NULL },
	{ "TEST_FILE(__n, __c) UnitTest::create(__ID__,"
		"__n, __c, # __c)", NULL },
	{ "PRINT_CONTENT(__n) UnitTest::strinfo(__ID__,"
		"UnitTest::content(__n), sprintf(\"content(%s)\", # __n))", NULL },
	{ "EXPECT_CONTENT(__n, __c) UnitTest::strcheck(__ID__, \"==\","
		"UnitTest::content(__n), __c, " 
		"sprintf(\"content(%s)\", # __n), # __c)", NULL },
	{ "MATCH_CONTENT(__n, __c) UnitTest::strcheck(__ID__, \"~\","
		"UnitTest::content(__n), __c, " 
		"sprintf(\"content(%s)\", # __n), # __c)", NULL },
	{ "EXPECT_REPR(__o, __r) UnitTest::strcheck(__ID__, \"==\", "
		"UnitTest::repr(__o), __r, "
		"sprintf(\"repr(%s)\", # __o), # __r)", NULL },
	{ "MATCH_REPR(__o, __r) UnitTest::strcheck(__ID__, \"~\", "
		"UnitTest::repr(__o), __r, "
		"sprintf(\"repr(%s)\", # __o), # __r)", NULL },
	{ "IN(__s) (IO)(__s)", NULL },
	{ "OUT() UnitTest::out_io()", NULL },
	{ "EXPECT_OUT(__r) UnitTest::strcheck(__ID__, \"==\", "
		"UnitTest::out_data(), __r, "
		"\"output()\", # __r)", NULL },
	{ "MATCH_OUT(__r) UnitTest::strcheck(__ID__, \"~\", "
		"UnitTest::out_data(), __r, "
		"\"output()\", # __r)", NULL },
	{ "TEST_FORK if (UnitTest::fork(__ID__))", NULL },
	{ "TEST_EXIT UnitTest::exit()", NULL },
	{ "CHILD_SIGNALED(__n) UnitTest::result(__ID__,"
		"UnitTest::wait() == -__n, \"child signaled %d\", __n)", NULL },
	{ "CHILD_EXIT(__n) UnitTest::result(__ID__,"
		"UnitTest::wait() == __n, \"child exit %d\", __n)", NULL },
};

static EfiParseDef pdef[] = {
	{ "TEST", PFunc_TestBlock, NULL },
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
	AddParseDef(pdef, tabsize(pdef));
	AddVarDef(GlobalVar, vardef, tabsize(vardef));
	PushVarTab(RefVarTab(UnitTestVar), NULL);
	AddVarDef(NULL, uvar, tabsize(uvar));
	AddFuncDef(fdef_func, tabsize(fdef_func));
	PopVarTab();
	AddMacDef(macdef, tabsize(macdef));
}
