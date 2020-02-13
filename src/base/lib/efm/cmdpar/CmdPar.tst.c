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

#define	MAX_NARG	20

static const char *eval_par(CmdPar *par, ...)
{
	static STRING_DECL(argbuf);
	static char *arg[MAX_NARG + 1];
	int narg;
	int n;

	va_list list;
	va_start(list, par);
	UnitTest_clear(&argbuf);

	for (narg = 0; narg < MAX_NARG; narg++)
	{
		const char *arg = va_arg(list, const char *);

		if	(!arg)
			break;

		UnitTest_puts(arg, &argbuf);
		UnitTest_putc(0, &argbuf);
	}

	UnitTest_putc(0, &argbuf);
	char *p = argbuf.data;

	for (n = 0; n < narg; n++, p++)
	{
		arg[n] = p;

		while (*p != 0)
			p++;
	}

	CmdPar_eval(par, &narg, arg, 0);

	IO *io = out();
	io_printf(io, "%d:", narg);

	for (n = 0; n < narg; n++)
		io_printf(io, " %s", arg[n]);

	return data();
}

TEST_DECL(read)
{
	CmdPar *par = CmdPar_alloc("test");
	test_par(par, "# CommandId\n\n"
		"# $Copyright Test-1.0\n"
		"# Comment\n"
		"\tSkip\n"
		"B = 1\n\tRDESC\n"
		"a|-aa|A = 6\n\tADESC\n"
		"/.*=.*/<name>=<value>|X\n\tXDESC\n"
		":c|C\n\tCDESC\n"
		"b:x|-bb::x|B\n\tBDESC\n"
		"@DENV|::d|D\n\tDDESC\n"
		"*args|\n"
		);

	EXPECT_STR(CmdPar_getval(par, "Ident", "X"), "CommandId\n");
	EXPECT_STR(CmdPar_getval(par, "Copyright", "X"),
		"Copyright Test-1.0");
	EXPECT_STR(CmdPar_getval(par, "A", "X"), "X");
	EXPECT_STR(CmdPar_getval(par, "B", "X"), "1");
	EXPECT_STR(CmdPar_getval(par, "C", "X"), "X");
	EXPECT_STR(CmdPar_getval(par, "D", "X"), "X");
	EXPECT_STR(CmdPar_getval(par, "X", "X"), "X");
	EXPECT_TRUE(par->var.used == 7);
	EXPECT_TRUE(par->def.used == 6);
	EXPECT_TRUE(par->env.used == 1);
	EXPECT_TRUE(par->opt.used == 4);
	EXPECT_TRUE(par->arg.used == 4);

	InfoNode *info = NewInfo(NULL, "info");
	CmdPar_info(par, info);

	CmdPar_docmode = 0;
	CmdPar_resource(par, out());
	EXPECT_STR(data(), "B = \"1\"\n\tRDESC\n");

	CmdPar_setval(par, "B", "a|b\t\"\n");
	CmdPar_docmode = 1;
	CmdPar_resource(par, out());
	EXPECT_STR(data(), "[|B = \"a||b\\t\\\"\\n\"|]\n\tRDESC\n");

	PrintInfo(out(), GetInfo(info, "Res"));
	EXPECT_STR(data(), "resources\nB = \"a|b\\t\\\"\\n\"\n\tRDESC\n");

	CmdPar_docmode = 0;
	CmdPar_synopsis(par, out(), 0);
	EXPECT_STR(data(), "test\
 [ -a ] [ --aa ] [ -b x ] [ --bb[=x] ]\
 { <name>=<value> }\
 c [ d ]\
\n");
	CmdPar_synopsis(par, out(), 1);
	EXPECT_STR(data(), "test\
 [ -a ] [ --aa ]\
 { <name>=<value> }\
 c [ -b x ] [ --bb[=x] ] [ d ]\
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
\\{~<|<name>=<value>|>~}\n\
</c/>\n\
\\[~</d/>~]\n\
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
<name>=<value>\n\tXDESC\n\
c\n\tCDESC\n\
-b x, --bb[=x]\n\tBDESC\n\
d\n\tDDESC\n\
");
	CmdPar_docmode = 1;
	CmdPar_arglist(par, out());
	EXPECT_STR(data(), "\
[<=|-a|, |--aa|=>]\n\tADESC\n\
[<=<|<name>=<value>|>=>]\n\tXDESC\n\
[<=</c/>=>]\n\tCDESC\n\
[<=|-b|~</x/>, |--bb|[|=|</x/>]=>]\n\tBDESC\n\
[<=</d/>=>]\n\tDDESC\n\
\\end\n");

	PrintInfo(out(), GetInfo(info, "Arg"));
	EXPECT_STR(data(), "options and arguments\n\
-a, --aa\n\tADESC\n\
<name>=<value>\n\tXDESC\n\
c\n\tCDESC\n\
-b x, --bb[=x]\n\tBDESC\n\
d\n\tDDESC\n\
");

	CmdPar_docmode = 0;
	CmdPar_environ(par, out());
	EXPECT_STR(data(), "DENV\n\tDDESC\n");

	CmdPar_docmode = 1;
	CmdPar_environ(par, out());
	EXPECT_STR(data(), "[|DENV|]\n\tDDESC\n");

	PrintInfo(out(), GetInfo(info, "Env"));
	EXPECT_STR(data(), "environment\nDENV\n\tDDESC\n");

	EXPECT_STR(eval_par(par, "name", "--", "-a", NULL), "1: name");
	EXPECT_STR(CmdPar_getval(par, "A", "X"), "X");
	EXPECT_STR(CmdPar_getval(par, "C", "X"), "-a");

	EXPECT_STR(eval_par(par, "name", "-a", "b", NULL), "1: name");
	EXPECT_STR(CmdPar_getval(par, "A", "X"), "6");
	EXPECT_STR(CmdPar_getval(par, "C", "X"), "b");

	EXPECT_STR(eval_par(par, "name", "-b", "z", "a", NULL), "1: name");
	EXPECT_STR(CmdPar_getval(par, "A", "X"), "6");
	EXPECT_STR(CmdPar_getval(par, "B", "X"), "z");
	EXPECT_STR(CmdPar_getval(par, "C", "X"), "a");


	EXPECT_STR(eval_par(par, "n", "a", NULL), "1: n");
	EXPECT_STR(CmdPar_getval(par, "C", "X"), "a");
	EXPECT_STR(CmdPar_getval(par, "D", "X"), "X");

	EXPECT_STR(eval_par(par, "n", "-", "b", NULL), "1: n");
	EXPECT_STR(CmdPar_getval(par, "C", "X"), "X");
	EXPECT_STR(CmdPar_getval(par, "D", "X"), "b");

	EXPECT_STR(eval_par(par, "n", "", NULL), "1: n");
	EXPECT_STR(CmdPar_getval(par, "C", "X"), "");
	EXPECT_STR(CmdPar_getval(par, "D", "X"), "b");

	EXPECT_STR(eval_par(par, "n", "x=y", "a", NULL), "1: n");
	EXPECT_STR(CmdPar_getval(par, "C", "X"), "a");
	EXPECT_STR(CmdPar_getval(par, "D", "X"), "b");
	EXPECT_STR(CmdPar_getval(par, "X", "X"), "x=y");

	EXPECT_STR(eval_par(par, "n", "a", "b", "c", NULL), "2: n c");
	EXPECT_STR(CmdPar_getval(par, "C", "X"), "a");
	EXPECT_STR(CmdPar_getval(par, "D", "X"), "b");

	TEST_FORK
	{
		setenv("DENV", "ENVDATA", 1);
		EXPECT_STR(eval_par(par, "n", "a", NULL), "1: n");
		EXPECT_STR(CmdPar_getval(par, "C", "X"), "a");
		EXPECT_STR(CmdPar_getval(par, "D", "X"), "ENVDATA");
		EXPECT_STR(eval_par(par, "n", "-", "b", NULL), "1: n");
		EXPECT_STR(CmdPar_getval(par, "C", "X"), "X");
		EXPECT_STR(CmdPar_getval(par, "D", "X"), "b");
		TEST_EXIT;
	}

	TEST_WAIT;

	/* TODO: Freigabe der Info-Struktur */
	rd_deref(info);
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

TEST_DECL(args)
{
	CmdPar *par = CmdPar_alloc("args");
	EXPECT_STR(cstr(rd_ident(par)), "args");
	test_par(par,
		"$last|L\n\tLast\n"
		"::opt|O\n\tOptional\n"
		"+extra|X: append \" \"\n\tExtra\n");

	EXPECT_STR(eval_par(par, "name", "a", NULL), "1: name");
	EXPECT_STR(CmdPar_getval(par, "O", NULL), NULL);
	EXPECT_STR(CmdPar_getval(par, "X", NULL), NULL);
	EXPECT_STR(CmdPar_getval(par, "L", NULL), "a");

	EXPECT_STR(eval_par(par, "name", "a", "b", NULL), "1: name");
	EXPECT_STR(CmdPar_getval(par, "O", NULL), "a");
	EXPECT_STR(CmdPar_getval(par, "X", NULL), NULL);
	EXPECT_STR(CmdPar_getval(par, "L", NULL), "b");

	CmdPar_setval(par, "X", NULL);
	EXPECT_STR(eval_par(par, "name", "a", "b", "c", NULL), "1: name");
	EXPECT_STR(CmdPar_getval(par, "O", NULL), "a");
	EXPECT_STR(CmdPar_getval(par, "X", NULL), "b");
	EXPECT_STR(CmdPar_getval(par, "L", NULL), "c");

	CmdPar_setval(par, "X", NULL);
	EXPECT_STR(eval_par(par, "name", "a", "b", "c", "d", NULL), "1: name");
	EXPECT_STR(CmdPar_getval(par, "O", NULL), "a");
	EXPECT_STR(CmdPar_getval(par, "X", NULL), "b c");
	EXPECT_STR(CmdPar_getval(par, "L", NULL), "d");

	CmdPar_setval(par, "O", "X");
	CmdPar_setval(par, "X", "X");
	CmdPar_setval(par, "L", "X");
	EXPECT_STR(eval_par(par, "name", "-", "-", "c", "-", NULL), "1: name");
	EXPECT_STR(CmdPar_getval(par, "O", NULL), NULL);
	EXPECT_STR(CmdPar_getval(par, "X", NULL), "X c");
	EXPECT_STR(CmdPar_getval(par, "L", NULL), NULL);

	CmdPar_docmode = 0;
	CmdPar_arglist(par, out());
	EXPECT_STR(data(), "\
last\n\tLast\n\
opt\n\tOptional\n\
extra\n\tExtra\n\
");
	rd_deref(par);
}

TEST_DECL(xargs)
{
	CmdPar *par = CmdPar_alloc("args");
	test_par(par,
		"/.*=.*/<name>=<value>|A\n\tAssign\n"
		"/@(.*)/<name>=<value>|B\n\tExtract\n"
		"*rest|\n\tRest\n");

	EXPECT_STR(eval_par(par, "name", "a", "b", NULL), "3: name a b");
	EXPECT_STR(CmdPar_getval(par, "A", NULL), NULL);
	EXPECT_STR(CmdPar_getval(par, "B", NULL), NULL);

	EXPECT_STR(eval_par(par, "name", "a=b", "@c", NULL), "1: name");
	EXPECT_STR(CmdPar_getval(par, "A", NULL), "a=b");
	EXPECT_STR(CmdPar_getval(par, "B", NULL), "c");

	EXPECT_STR(eval_par(par, "name", "@a", "x", "b=c", NULL), "2: name x");
	EXPECT_STR(CmdPar_getval(par, "A", NULL), "b=c");
	EXPECT_STR(CmdPar_getval(par, "B", NULL), "a");

	rd_deref(par);
}

TEST_DECL(env)
{
	CmdPar *par = CmdPar_alloc("args");
	test_par(par,
		"@TEST_1|A\n\tADESC\n"
		"@TEST_2[default]|B\n\tBDESC\n"
		);

	unsetenv("TEST_1");
	unsetenv("TEST_2");
	EXPECT_STR(eval_par(par, "name", NULL), "1: name");
	EXPECT_STR(CmdPar_getval(par, "A", NULL), NULL);
	EXPECT_STR(CmdPar_getval(par, "B", NULL), "default");

	setenv("TEST_1", "a", 1);
	setenv("TEST_2", "b", 1);
	EXPECT_STR(eval_par(par, "name", NULL), "1: name");
	EXPECT_STR(CmdPar_getval(par, "A", NULL), "a");
	EXPECT_STR(CmdPar_getval(par, "B", NULL), "b");

	rd_deref(par);
}

TEST_SUITE
{
	TEST_RUN(Static);
	TEST_RUN(Dynamic);
	TEST_RUN(Ptr);
	TEST_RUN(read);
	TEST_RUN(args);
	TEST_RUN(xargs);
	TEST_RUN(env);
}
