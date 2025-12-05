#include <EFEU/UnitTest.h>
#include <EFEU/preproc.h>
#include <EFEU/utest/in.tst.h>
#include <EFEU/utest/out.tst.h>

static const char *define(const char *def)
{
	PPMacro *macro = ParseMacro(IN(def));
	AddMacro(macro);
	ShowMacro(OUT(), macro);
	return utest_out();
}

static const char *subst(const char *def)
{
	io_macsub(IN(def), OUT(), NULL);
	return utest_out(); 
}

static int visit(const char *name, void *data, void *par)
{
	io_printf(par, "%s\n", name);
	return 0;
}

static const char *maclist(void)
{
	nkt_walk(MacroTab, visit, OUT());
	return utest_out(); 
}

#define	MACDEF(def)	PRINT_STR(define(def))
#define	MACSUB(in,out)	EXPECT_STR(subst(in), out)
#define	MACLIST(list)	EXPECT_STR(maclist(), list)
#define	CLEANUP_MAC	nkt_clean(MacroTab)

TEST_DECL(stack)
{
	NameKeyTab *tab;

	MACDEF("A a");
	MACDEF("B b");
	MACSUB("A B C", "a b C");
	MACLIST("A\nB\n");

	TEST_EXPR(PushMacroTab(NULL));
	MACDEF("A x");
	MACLIST("A\n");
	MACSUB("A B C", "x b C");

	EXPECT_PTR(tab = PopMacroTab());
	MACLIST("A\nB\n");
	TEST_EXPR(PushMacroTab(tab));
	MACLIST("A\n");
	EXPECT_PTR(tab = PopMacroTab());
	TEST_EXPR(nkt_clean(tab));
	MACLIST("A\nB\n");

	EXPECT(PopMacroTab() == NULL);
	CLEANUP_MAC;
}

static int m_sub (PPMacro *mac, IO *io, char **arg, int narg)
{
	int n = io_printf(io, "%s[%d](%s)", mac->name, mac->dim, mac->repl);
	n += io_printf(io, "[%d]:", narg);

	for (int i = 0; i < narg; ++i)
		n += io_printf(io, "<%s>", arg[i]);

	return n;
}

TEST_DECL(macdef)
{
	PPMacDef def[] = {
		{ "A(x) x", NULL },
		{ "B macro b", m_sub },
		{ "C(x,y) macro c", m_sub },
		{ "D(...) macro d", m_sub },
		{ "def(x)", macsub_def },
	};

	AddMacDef(def, sizeof def / sizeof def[0]);
	MACLIST("A\nB\nC\nD\ndef\n");
	MACSUB("{A(x)}", "{x}");
	MACSUB("{B}", "{B[0](macro b)[0]:}");
	MACSUB("{C()}", "{C[2](macro c)[1]:<>}");
	MACSUB("{C(a)}", "{C[2](macro c)[1]:<a>}");
	MACSUB("{C(a,b)}", "{C[2](macro c)[2]:<a><b>}");
	MACSUB("{C(,)}", "{C[2](macro c)[2]:<><>}");
	MACSUB("{C([,])}", "{C[2](macro c)[1]:<[,]>}");
	MACSUB("{C({,})}", "{C[2](macro c)[1]:<{,}>}");
	MACSUB("{C(\",\")}", "{C[2](macro c)[1]:<\",\">}");
	MACSUB("{C(',')}", "{C[2](macro c)[1]:<','>}");

	MACSUB("{D()}", "{D[1](macro d)[1]:<>}");
	MACSUB("{D(a)}", "{D[1](macro d)[1]:<a>}");
	MACSUB("{D(a,b)}", "{D[1](macro d)[1]:<a,b>}");
	MACSUB("{def(A)}", "{1}");
	MACSUB("{def(X)}", "{0}");

	CLEANUP_MAC;
}

TEST_DECL(redef)
{
	SetApplPath(UnitTest_tmp(NULL));
	TEST_FILE("preproc.msg", "211	NOTE 211.\n");

	MACDEF("A v1");
	MACLIST("A\n");
	MACSUB("{A}", "{v1}");

	UnitTest_stderr("stderr");
	MACDEF("A v2");
	UnitTest_stderr(NULL);
	EXPECT_CONTENT("stderr", "MSG NOTE 211.\n");
	MACLIST("A\n");
	MACSUB("{A}", "{v2}");

	CLEANUP_MAC;
}

TEST_DECL(noarg)
{
	MACDEF("A1 [a\\\nb\\c]\nd");
	MACDEF("A2 [ a ]\\");
	MACLIST("A1\nA2\n");

	MACSUB("{A1}", "{[ab\\c]}");
	MACSUB("{A2}", "{[ a ]\\}");
	MACSUB("{A2()}", "{[ a ]\\()}");

	CLEANUP_MAC;
}

TEST_DECL(blank)
{
	MACDEF("B() [foo]");
	MACLIST("B\n");

	MACSUB("{B ( )}", "{[foo]}");
	MACSUB("{B(1)}", "{[foo]}");

	CLEANUP_MAC;
}

TEST_DECL(single)
{
	MACDEF("C(x) [x]");
	MACLIST("C\n");

	MACSUB("{C()}", "{[]}");
	MACSUB("{C(1)}", "{[1]}");
	MACSUB("{C ( 1 )}", "{[1 ]}");
	MACSUB("{C(1, 2)}", "{[1]}");
	MACSUB("{C ( 1 , 2)}", "{[1 ]}");
	MACSUB("{C ((1, 2))}", "{[(1, 2)]}");

	CLEANUP_MAC;
}

TEST_DECL(vaarg)
{
	MACDEF("D(...) [va_list]");
	MACLIST("D\n");

	MACSUB("{D()}", "{[]}");
	MACSUB("{D(1)}", "{[1]}");
	MACSUB("{D(1, 2)}", "{[1, 2]}");

	CLEANUP_MAC;
}

TEST_DECL(mixed)
{
	MACDEF("E(x, ...) [x:va_list]");
	MACLIST("E\n");

	MACSUB("{E()}", "{[:]}");
	MACSUB("{E(1)}", "{[1:]}");
	MACSUB("{E(1, 2)}", "{[1:2]}");
	MACSUB("{E(1, 2, 3)}", "{[1:2, 3]}");
	MACSUB("{E((1, 2), 3, 4)}", "{[(1, 2):3, 4]}");

	CLEANUP_MAC;
}

TEST_DECL(string)
{
	MACDEF("A(x) [x \"x\" # x \"# x\"]");
	MACLIST("A\n");

	MACSUB("{A(z)}", "{[z \"x\" \"z\" \"# x\"]}");
	MACSUB("{\"A(z)\"}", "{\"A(z)\"}");
	MACSUB("{'A(z)'}", "{'A(z)'}");
	CLEANUP_MAC;
}

TEST_DECL(lock)
{
	MACDEF("A [A B]");
	MACSUB("{A}", "{[A B]}");
	MACDEF("B [B C]");
	MACSUB("{A}", "{[A [B C]]}");
	MACSUB("{B}", "{[B C]}");
	MACDEF("C [C A]");
	MACSUB("{A}", "{[A [B [C A]]]}");
	MACSUB("{B}", "{[B [C [A B]]]}");
	MACSUB("{C}", "{[C [A [B C]]]}");

	CLEANUP_MAC;
}

TEST_DECL(paste)
{
	MACDEF("A(x,y,z) [ x ## y # z ]");
	MACDEF("B(x,y) [ x#y ]");
	MACDEF("C(x) [ x ## . ]");
	MACDEF("X2 [x2]");
	EXPECT_STR(GetMacro("A")->repl, "[ x##y # z ]");
	EXPECT_STR(GetMacro("B")->repl, "[ x#y ]");
	EXPECT_STR(GetMacro("C")->repl, "[ x##. ]");
	MACSUB("{A(1,2,3)}", "{[ 12 \"3\" ]}");
	MACSUB("{A(X,1)}", "{[ X1 \"\" ]}");
	MACSUB("{A(X,2,3)}", "{[ [x2] \"3\" ]}");
	MACSUB("{B(1,2)}", "{[ 1\"2\" ]}");
	MACSUB("{C(x)}", "{[ x. ]}");

	CLEANUP_MAC;
}

TEST_SUITE
{
	CLEANUP_MAC;
	MACLIST("");

	TEST_RUN(stack);
	TEST_RUN(macdef);
	TEST_RUN(redef);
	TEST_RUN(noarg);
	TEST_RUN(blank);
	TEST_RUN(single);
	TEST_RUN(vaarg);
	TEST_RUN(mixed);
	TEST_RUN(string);
	TEST_RUN(lock);
	TEST_RUN(paste);

	CLEANUP_IN;
	CLEANUP_OUT;
}
