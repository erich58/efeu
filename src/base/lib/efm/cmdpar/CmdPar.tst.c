#include <EFEU/UnitTest.h>
#include <EFEU/CmdPar.h>
#include "utest/data.tst.h"

TEST_DECL(Static)
{
	static CmdPar par;
	CmdPar_init(&par);
	EXPECT_TRUE(par.refcount == 0);
	CmdPar_clean(&par);
}

TEST_DECL(Dynamic)
{
	CmdPar *par = CmdPar_alloc("dummy");
	EXPECT_STR(cstr(rd_ident(par)), "dummy");
	EXPECT_TRUE(par->refcount == 1);
	rd_deref(par);
}

static void test_par(CmdPar *par, const char *str)
{
	IO *io = io_cstr(str);
	CmdPar_read(par, io, EOF, 1);
	io_close(io);
}

TEST_DECL(read)
{
	CmdPar *par = CmdPar_alloc("test");
	test_par(par, "#OPT\nB = 1\n"
		"a|-aa|A = 6\n\tADESC\n"
		"@CENV|:c|C\n\tCDESC\n"
		"b:x|-bb::x|B\n\tBDESC\n"
		"::d|D\n\tDDESC\n"
		"/.*=.*/<name>=<value>|\n\tXDESC\n"
		);

	EXPECT_STR(CmdPar_getval(par, "Ident", "X"), "OPT\n");
	EXPECT_STR(CmdPar_getval(par, "A", "X"), "X");
	EXPECT_STR(CmdPar_getval(par, "B", "X"), "1");
	EXPECT_STR(CmdPar_getval(par, "C", "X"), "X");
	EXPECT_STR(CmdPar_getval(par, "D", "X"), "X");
	EXPECT_TRUE(par->var.used == 5);
	EXPECT_TRUE(par->def.used == 5);
	EXPECT_TRUE(par->env.used == 1);
	EXPECT_TRUE(par->opt.used == 4);
	EXPECT_TRUE(par->arg.used == 3);

	CmdPar_docmode = 0;
	CmdPar_synopsis(par, out(), 0);
	EXPECT_STR(data(), "test\
 [ -a ] [ --aa ] [ -b x ] [ --bb[=x] ] c [ d ]\
 { <name>=<value> }\
\n");
	CmdPar_synopsis(par, out(), 1);
	EXPECT_STR(data(), "test\
 [ -a ] [ --aa ] c [ -b x ] [ --bb[=x] ] [ d ]\
 { <name>=<value> }\
\n");
	CmdPar_docmode = 1;
	CmdPar_synopsis(par, out(), 0);
	EXPECT_STR(data(), "\
\\hang\n\
|test|\n\
\\[~|-a|~]\n\
\\[~|--aa|~]\n\
\\[~|-b|~</x/>~]\n\
\\[~|--bb|[|=|</x/>]~]\n\
</c/>\n\
\\[~</d/>~]\n\
\\{~<|<name>=<value>|>~}\n\
\\end\n");

	CmdPar_docmode = 0;
	CmdPar_options(par, out());
	EXPECT_STR(data(), "\
-a, --aa\n\tADESC\n\
-b x, --bb[=x]\n\tBDESC\n\
");
	CmdPar_arglist(par, out());
	EXPECT_STR(data(), "\
-a, --aa\n\tADESC\n\
c\n\tCDESC\n\
-b x, --bb[=x]\n\tBDESC\n\
d\n\tDDESC\n\
<name>=<value>\n\tXDESC\n\
");
	CmdPar_docmode = 1;
	CmdPar_arglist(par, out());
	EXPECT_STR(data(), "\
[<=|-a|, |--aa|=>]\n\tADESC\n\
[<=</c/>=>]\n\tCDESC\n\
[<=|-b|~</x/>, |--bb|[|=|</x/>]=>]\n\tBDESC\n\
[<=</d/>=>]\n\tDDESC\n\
[<=<|<name>=<value>|>=>]\n\tXDESC\n\
\\end\n");

	CmdPar_docmode = 0;
	CmdPar_environ(par, out());
	EXPECT_STR(data(), "CENV\n\tCDESC\n");

	CmdPar_docmode = 1;
	CmdPar_environ(par, out());
	EXPECT_STR(data(), "[|CENV|]\n\tCDESC\n");

//	CmdPar_eval(par, 0, NULL, 0);
	rd_deref(par);
}

TEST_DECL(Ptr)
{
	CmdPar *par = NULL;
	EXPECT_TRUE((par = CmdPar_ptr(NULL)) != NULL);
	EXPECT_TRUE(par->refcount == 1);
	EXPECT_TRUE(par == CmdPar_ptr(NULL));
	CmdPar_setval(par, NULL, "CmdPar");
	EXPECT_STR(cstr(rd_ident(par)), "CmdPar");

	CmdPar_setval(par, "a", "A");
	CmdPar_setval(NULL, "b", "B");
	EXPECT_TRUE(par->var.used == 2); 

	EXPECT_STR(CmdPar_getval(par, "b", "X"), "B");
	EXPECT_STR(CmdPar_getval(NULL, "b", "X"), "B");
	EXPECT_STR(CmdPar_getval(par, "c", "X"), "X");
	EXPECT_STR(CmdPar_getval(par, NULL, "X"), "CmdPar");
	CmdPar_showval(par, out(), NULL);
	EXPECT_STR(data(), "a=\"A\"\nb=\"B\"\n");
	CmdPar_showval(par, out(), "$1:$2;");
	EXPECT_STR(data(), "a:A;b:B;");

	CmdParDef *def = CmdParDef_alloc();
	CmdPar_add(par, def);
	EXPECT_TRUE(par->def.used == 1); 
	EXPECT_TRUE(par->arg.used == 0); 
	EXPECT_TRUE(par->opt.used == 0); 
	EXPECT_TRUE(par->env.used == 0); 

	InfoNode *info = NewInfo(NULL, "CmdPar");
	CmdPar_info(par, info);
	PrintInfo(out(), info);
	EXPECT_MATCH(data(), "\nArg\t.*\nEnv\t.*\nRes\t.*\n");

	CmdPar_var(par, "a", 1)->desc = "Description";
	PrintInfo(out(), GetInfo(info, "Res"));
	EXPECT_STR(data(), "resources\na = \"A\"\n\tDescription");

	CmdPar_clean(par);
	EXPECT_TRUE(par->var.size == 0); 
}

TEST_SUITE
{
	TEST_RUN(Static);
	TEST_RUN(Dynamic);
	TEST_RUN(Ptr);
	TEST_RUN(read);
}
