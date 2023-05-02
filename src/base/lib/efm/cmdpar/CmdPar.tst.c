#include <EFEU/UnitTest.h>
#include <EFEU/CmdPar.h>
#include <EFEU/Debug.h>
#include <EFEU/mstring.h>
#include "utest/out.tst.h"
#include "utest/mstr.tst.h"

static CmdPar *par = NULL;

static void pset(const char *name, const char *value)
{
	CmdPar_setval(par, name, mstrcpy(value));
}

static const char *pget(const char *name)
{
	return CmdPar_getval(par, name, NULL);
}

TEST_DECL(setup)
{
	par = CmdPar_alloc(NULL);
	rd_deref(par);

	par = CmdPar_alloc("test");
	GetFormatDisable = 1;
}

static void pconfig(const char *str)
{
	IO *io = io_cstr(str);
	CmdPar_read(par, io, EOF, 1);
	io_close(io);
}

#define	MAX_NARG	20

static const char *eval_par(const char *first)
{
	static STRING_DECL(argbuf);
	static char *arg[MAX_NARG + 1];
	int narg;
	int n;

	UnitTest_clear(&argbuf);

	for (narg = 0; *first; ++first)
	{
		if	(*first == ' ')
		{
			UnitTest_putc(0, &argbuf);

			if	(++narg >= MAX_NARG)
				break;
		}
		else
		{
			UnitTest_putc(*first, &argbuf);
		}
	}

	UnitTest_putc(0, &argbuf);
	++narg;
	UnitTest_putc(0, &argbuf);

	char *p = argbuf.data;

	for (n = 0; n < narg; n++, p++)
	{
		arg[n] = p;

		while (*p != 0)
			p++;
	}

	UnitTest_stderr("error.log");
	int stat = CmdPar_eval(par, &narg, arg, 0);
	UnitTest_stderr(NULL);

	IO *io = OUT();

	if	(stat < 0)
		io_puts("ERR", io);
	else
		io_printf(io, "%d", stat);

	for (n = 0; n < narg; n++)
		io_printf(io, " %s", arg[n]);

	return utest_out();
}

TEST_DECL(read)
{
	pconfig(
		"# CommandId\n\n"
		"# $Copyright Test-1.0\n"
		"# Comment\n"
		"\tSkip\n"
		"B = 1\n\tRDESC\n"
		"a|-aa|A = 6\n\tADESC\n"
		"/.*=.*/<name>=<value>|X\n\tXDESC\n"
		":c|C\n\tCDESC\n"
		"b:x|-bb::x|B\n\tBDESC\n"
		"@DENV|::d|D\n\t:*:DDESC\n"
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
	EXPECT(par->var.used == 7);
	EXPECT(par->def.used == 6);
	EXPECT(par->env.used == 1);
	EXPECT(par->opt.used == 4);
	EXPECT(par->arg.used == 4);

	InfoNode *info = NewInfo(NULL, "info");
	CmdPar_info(par, info);

	CmdPar_docmode = 0;
	CmdPar_resource(par, OUT());
	EXPECT_OUT("B = \"1\"\n\tRDESC\n");

	CmdPar_setval(par, "B", "a|b\t\"\n");
	CmdPar_docmode = 1;
	CmdPar_resource(par, OUT());
	EXPECT_OUT("[|B = \"a||b\\t\\\"\\n\"|]\n\tRDESC\n");

	PrintInfo(OUT(), GetInfo(info, "Res"));
	EXPECT_OUT("resources\nB = \"a|b\\t\\\"\\n\"\n\tRDESC\n");

	CmdPar_docmode = 0;
	CmdPar_synopsis(par, OUT(), 0);
	EXPECT_OUT("test\
 [ -a ] [ --aa ] [ -b x ] [ --bb[=x] ]\
 { <name>=<value> }\
 c [ d ]\
\n");
	CmdPar_synopsis(par, OUT(), 1);
	EXPECT_OUT("test\
 [ -a ] [ --aa ]\
 { <name>=<value> }\
 c [ -b x ] [ --bb[=x] ] [ d ]\
\n");
	CmdPar_docmode = 1;
	CmdPar_synopsis(par, OUT(), 0);
	EXPECT_OUT("\
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
	CmdPar_options(par, OUT());
	EXPECT_OUT("\
-a, --aa\n\tADESC\n\
-b x, --bb[=x]\n\tBDESC\n\
");
	CmdPar_arglist(par, OUT());
	EXPECT_OUT("\
-a, --aa\n\tADESC\n\
<name>=<value>\n\tXDESC\n\
c\n\tCDESC\n\
-b x, --bb[=x]\n\tBDESC\n\
d\n\tDDESC\n\
");
	CmdPar_docmode = 1;
	CmdPar_arglist(par, OUT());
	EXPECT_OUT("\
[<=|-a|, |--aa|=>]\n\tADESC\n\
[<=<|<name>=<value>|>=>]\n\tXDESC\n\
[<=</c/>=>]\n\tCDESC\n\
[<=|-b|~</x/>, |--bb|[|=|</x/>]=>]\n\tBDESC\n\
[<=</d/>=>]\n\tDDESC\n\
\\end\n");

	PrintInfo(OUT(), GetInfo(info, "Arg"));
	EXPECT_OUT("options and arguments\n\
-a, --aa\n\tADESC\n\
<name>=<value>\n\tXDESC\n\
c\n\tCDESC\n\
-b x, --bb[=x]\n\tBDESC\n\
d\n\tDDESC\n\
");

	CmdPar_docmode = 0;
	CmdPar_environ(par, OUT());
	EXPECT_OUT("DENV\n\tDDESC\n");

	CmdPar_docmode = 1;
	CmdPar_environ(par, OUT());
	EXPECT_OUT("[|DENV|]\n\tDDESC\n");

	PrintInfo(OUT(), GetInfo(info, "Env"));
	EXPECT_OUT("environment\nDENV\n\tDDESC\n");

	/* TODO: Freigabe der Info-Struktur */
	// rd_deref(info);
	CmdPar_clean(par);
}

TEST_DECL(Ptr)
{
	CmdPar *par = NULL;
	EXPECT((par = CmdPar_ptr(NULL)) != NULL);
	EXPECT(par->refcount == 1);
	EXPECT(par == CmdPar_ptr(NULL));
	CmdPar_setval(par, NULL, "CmdPar");
	EXPECT_MSTR(rd_ident(par), "CmdPar");

	CmdPar_setval(par, "a", "A");
	CmdPar_setval(par, "b", "B");
	EXPECT(par->var.used == 2); 

	EXPECT_STR(CmdPar_getval(par, "b", "X"), "B");
	EXPECT_STR(CmdPar_getval(NULL, "b", "X"), "B");
	EXPECT_STR(CmdPar_getval(par, "c", "X"), "X");
	EXPECT_STR(CmdPar_getval(par, NULL, "X"), "CmdPar");
	CmdPar_showval(par, OUT(), NULL);
	EXPECT_OUT("a=\"A\"\nb=\"B\"\n");
	CmdPar_showval(par, OUT(), "$1:$2;");
	EXPECT_OUT("a:A;b:B;");

	CmdParDef *def = CmdParDef_alloc();
	CmdPar_add(par, def);
	EXPECT(par->def.used == 1); 
	EXPECT(par->arg.used == 0); 
	EXPECT(par->opt.used == 0); 
	EXPECT(par->env.used == 0); 

	InfoNode *info = NewInfo(NULL, "CmdPar");
	CmdPar_info(par, info);
	PrintInfo(OUT(), info);
	MATCH_OUT("\nArg\t.*\nEnv\t.*\nRes\t.*\n");

	CmdPar_var(par, "a", 1)->desc = "Description";
	PrintInfo(OUT(), GetInfo(info, "Res"));
	EXPECT_OUT("resources\na = \"A\"\n\tDescription");

	CmdPar_clean(par);
	EXPECT(par->var.size == 0); 
}

TEST_DECL(opt)
{
	pconfig(
		"a[6]|-la[8]|A\n\tADESC\n"
		"b:x|-ob:x|B\n\tBDESC\n"
		"c::x[C1]|-ocd::x[C2]|C\n\tCDESC\n"
		"*rest|\n\tRest\n"
	);

	pset("A", "X");
	pset("B", "X");
	EXPECT_STR(eval_par("name x"), "2 name x");
	EXPECT_STR(pget("A"), "X");
	EXPECT_STR(pget("B"), "X");

	pset("A", "X");
	EXPECT_STR(eval_par("name -a x y"), "3 name x y");
	EXPECT_STR(pget("A"), "6");

	pset("A", "X");
	pset("B", "X");
	EXPECT_STR(eval_par("name -ab x y"), "2 name y");
	EXPECT_STR(pget("A"), "6");
	EXPECT_STR(pget("B"), "x");

	pset("A", "X");
	pset("B", "X");
	EXPECT_STR(eval_par("name -ba x"), "2 name x");
	EXPECT_STR(pget("A"), "X");
	EXPECT_STR(pget("B"), "a");

	pset("B", "X");
	EXPECT_STR(eval_par("name -b"), "ERR name");
	EXPECT_CONTENT("error.log", "MSG [efm:31] $0=\"\" $1=\"b\"\n");
	EXPECT_STR(pget("B"), "X");

	pset("C", "X");
	EXPECT_STR(eval_par("name -c"), "1 name");
	EXPECT_STR(pget("C"), "C1");
	EXPECT_CONTENT("error.log", "");

	pset("B", "X");
	EXPECT_STR(eval_par("name -b - x"), "2 name x");
	EXPECT_STR(pget("B"), NULL);

	pset("C", "X");
	EXPECT_STR(eval_par("name -c - x"), "3 name - x");
	EXPECT_STR(pget("C"), "C1");

	pset("B", "X");
	EXPECT_STR(eval_par("name -b \\- x"), "2 name x");
	EXPECT_STR(pget("B"), "-");

	pset("C", "X");
	EXPECT_STR(eval_par("name -ca x"), "2 name x");
	EXPECT_STR(pget("C"), "a");

	EXPECT_STR(eval_par("name -x a b"), "ERR name a b");
	EXPECT_CONTENT("error.log", "MSG [efm:32] $0=\"\" $1=\"x\"\n");

	EXPECT_STR(eval_par("name --o a b"), "ERR name a b");
	EXPECT_CONTENT("error.log",
		"MSG [efm:33] $0=\"\" $1=\"-o\" $2=\"-ob\" $3=\"-ocd\"\n");

	pset("B", "X");
	EXPECT_STR(eval_par("name --ob a x"), "2 name x");
	EXPECT_STR(pget("B"), "a");

	pset("B", "X");
	EXPECT_STR(eval_par("name --ob=a x"), "2 name x");
	EXPECT_STR(pget("B"), "a");

	pset("B", "X");
	EXPECT_STR(eval_par("name --ob= a x"), "3 name a x");
	EXPECT_STR(pget("B"), "");

	pset("B", "X");
	EXPECT_STR(eval_par("name --ob=- a x"), "3 name a x");
	EXPECT_STR(pget("B"), NULL);

	pset("C", "X");
	EXPECT_STR(eval_par("name --oc a x"), "3 name a x");
	EXPECT_STR(pget("C"), "C2");

	pset("C", "X");
	EXPECT_STR(eval_par("name --oc=a x"), "2 name x");
	EXPECT_STR(pget("C"), "a");

	pset("C", "X");
	EXPECT_STR(eval_par("name --oc= x"), "2 name x");
	EXPECT_STR(pget("C"), "");

	pset("C", "X");
	EXPECT_STR(eval_par("name --oc=- x"), "2 name x");
	EXPECT_STR(pget("C"), "C2");

	pset("A", "X");
	EXPECT_STR(eval_par("name --la x y"), "3 name x y");
	EXPECT_STR(pget("A"), "8");

	pset("B", "X");
	EXPECT_STR(eval_par("name --obx y"), "ERR name y");
	EXPECT_CONTENT("error.log", "MSG [efm:34] $0=\"\" $1=\"--obx\"\n");
	EXPECT_STR(pget("B"), "X");

	CmdPar_clean(par);
}

TEST_DECL(arg)
{
	pconfig(
		":std|S\n\tStandard\n"
		"::opt|O\n\tOptional\n"
		"*rest|\n\tRest\n"
	);

	EXPECT_STR(eval_par("name x"), "1 name");
	EXPECT_STR(pget("S"), "x");
	EXPECT_STR(pget("O"), NULL);

	EXPECT_STR(eval_par("name x y"), "1 name");
	EXPECT_STR(pget("S"), "x");
	EXPECT_STR(pget("O"), "y");

	EXPECT_STR(eval_par("name x y z"), "2 name z");
	EXPECT_STR(pget("S"), "x");
	EXPECT_STR(pget("O"), "y");

	EXPECT_STR(eval_par("name - -- y z"), "2 name z");
	EXPECT_STR(pget("S"), NULL);
	EXPECT_STR(pget("O"), "y");

	CmdPar_clean(par);
}

TEST_DECL(xargs)
{
	pconfig(
		"$last|L\n\tLast\n"
		"::opt|O\n\tOptional\n"
		"+extra|X: append \" \"\n\tExtra\n"
	);

	CmdPar_docmode = 0;
	pset(NULL, "test");
	pset(".synopsis.head", NULL);

	EXPECT_STR(eval_par("name a b"), "0 name");
	EXPECT_CONTENT("error.log", "test [ opt ] extra last\n");
	EXPECT_STR(pget("O"), "a");
	EXPECT_STR(pget("X"), NULL);
	EXPECT_STR(pget("L"), "b");

	pset("X", NULL);
	EXPECT_STR(eval_par("name a b c"), "1 name");
	EXPECT_CONTENT("error.log", "");
	EXPECT_STR(pget("O"), "a");
	EXPECT_STR(pget("X"), "b");
	EXPECT_STR(pget("L"), "c");

	pset("X", NULL);
	EXPECT_STR(eval_par("name a b c d"), "1 name");
	EXPECT_STR(pget("O"), "a");
	EXPECT_STR(pget("X"), "b c");
	EXPECT_STR(pget("L"), "d");

	pset("O", "X");
	pset("X", "X");
	pset("L", "X");
	EXPECT_STR(eval_par("name - - c -"), "1 name");
	EXPECT_STR(pget("O"), NULL);
	EXPECT_STR(pget("X"), "X c");
	EXPECT_STR(pget("L"), NULL);

	CmdPar_docmode = 0;
	CmdPar_arglist(par, OUT());
	EXPECT_OUT("\
last\n\tLast\n\
opt\n\tOptional\n\
extra\n\tExtra\n\
");
	CmdPar_clean(par);
}

TEST_DECL(regex)
{
	pconfig(
		"/.*=.*/<name>=<value>|A\n\tAssign\n"
		"/@(.*)/<name>=<value>|B\n\tExtract\n"
		"*rest|\n\tRest\n"
	);

	EXPECT_STR(eval_par("name a b"), "3 name a b");
	EXPECT_STR(pget("A"), NULL);
	EXPECT_STR(pget("B"), NULL);

	EXPECT_STR(eval_par("name a=b @c"), "1 name");
	EXPECT_STR(pget("A"), "a=b");
	EXPECT_STR(pget("B"), "c");

	EXPECT_STR(eval_par("name @a x b=c"), "2 name x");
	EXPECT_STR(pget("A"), "b=c");
	EXPECT_STR(pget("B"), "a");

	CmdPar_clean(par);
}

TEST_DECL(env)
{
	pconfig(
		"@TEST_1|A\n\tADESC\n"
		"@TEST_2[default]|B\n\tBDESC\n"
	);

	unsetenv("TEST_1");
	unsetenv("TEST_2");
	EXPECT_STR(eval_par("name"), "1 name");
	EXPECT_STR(pget("A"), NULL);
	EXPECT_STR(pget("B"), "default");

	setenv("TEST_1", "a", 1);
	setenv("TEST_2", "b", 1);
	EXPECT_STR(eval_par("name"), "1 name");
	EXPECT_STR(pget("A"), "a");
	EXPECT_STR(pget("B"), "b");

	CmdPar_clean(par);
}

TEST_DECL(syntax)
{
	pconfig(
		"o|O\n\tOption\n"
		":arg|A\n\tArg\n"
	);

	CmdPar_docmode = 0;
	pset(NULL, "test");
	pset(".synopsis.intro", "usage: ");

	setenv("PATH", UnitTest_tmp(NULL), 1);
	EXPECT_STR(eval_par("name"), "0 name");
	EXPECT_CONTENT("error.log", "test [ -o ] arg\n");

	EXPECT_STR(eval_par("name a b c"), "ERR name b c");
	EXPECT_CONTENT("error.log", "test [ -o ] arg\n");

	UnitTest_mkfile("syntax.log", NULL);

	TEST_FORK
	{
		ioerr = io_fopen(UnitTest_tmp("syntax.log"), "w");
		setenv("PATH", UnitTest_tmp(NULL), 1);
		eval_par("name -?");
		// not reached
		exit(99);
	}

	CHILD_EXIT(0);
	EXPECT_CONTENT("syntax.log", "test [ -o ] arg\n");

	CmdPar_clean(par);
}

TEST_DECL(write)
{
	pset("@A\tB\nC:D", "X");
	pconfig(
		"@ENV|\n"
		"o|-opt[x]|\n\tOpt\n"
		":a|A:set x\n"
		"::b|B = z\n"
		"+c|\n"
		"*d|\n"
		"/e/|\n"
		"$f|\n"
	);

	CmdPar_write(par, OUT());
	EXPECT_OUT(
		"\\@A\\tB\\nC\\:D = \"X\"\n"
		"A = NULL\n"
		"B = NULL\n"
		"Ident = NULL\n\n"
		"@ENV|\n"
		"o|-opt[x]|\n\tOpt\n"
		":a|A: set \"x\"\n"
		"::b|B = \"z\"\n"
		"+c|\n"
		"*d|\n"
		"/e/|\n"
		"$f|\n"
	);
}

TEST_SUITE
{
	TEST_RUN(setup);
	TEST_RUN(Ptr);
	TEST_RUN(read);
	TEST_RUN(xargs);
	TEST_RUN(regex);
	TEST_RUN(env);
	TEST_RUN(arg);
	TEST_RUN(opt);
	TEST_RUN(syntax);
	TEST_RUN(write);
	CLEANUP_MSTR;
	CLEANUP_OUT;
}
