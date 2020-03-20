#include <EFEU/UnitTest.h>
#include <EFEU/parsearg.h>
#include "utest/data.tst.h"

TEST_DECL(argval)
{
	EXPECT_STR(argval(NULL), NULL);
	EXPECT_STR(argval("-"), NULL);
	EXPECT_STR(cstr(argval("x")), "x");
}

static const char *argl(int narg, char **arg)
{
	IO *io = out();
	int n;

	io_printf(io, "%d:", narg);

	for (n = 0; n < narg; n++)
		io_printf(io, " %s", arg[n]);

	return data();
}

TEST_DECL(skiparg)
{
	char *arg[] = { "a", "b", "c", "d", "e", NULL };
	int n = tabsize(arg) - 1;
	EXPECT_STR(argl(n, arg), "5: a b c d e"); 

	skiparg(&n, arg, n); 
	EXPECT_STR(argl(n, arg), "5: a b c d e"); 
	skiparg(&n, arg, -1); 
	EXPECT_STR(argl(n, arg), "5: a b c d e"); 
	skiparg(&n, arg, n - 1); 
	EXPECT_STR(argl(n, arg), "4: a b c d"); 
	skiparg(&n, arg, 2); 
	EXPECT_STR(argl(n, arg), "3: a b d"); 
	skiparg(&n, arg, 0); 
	EXPECT_STR(argl(n, arg), "2: b d"); 
}

TEST_DECL(assignarg)
{
	char *rest;
	AssignArg *arg;

	rest = "X";
	EXPECT(assignarg(NULL, &rest, NULL) == NULL);
	EXPECT_STR(rest, NULL);

	rest = "X";
	EXPECT(assignarg(" \t", &rest, NULL) == NULL);
	EXPECT_STR(rest, NULL);

#define	ASSIGNARG(def, d, n, o, a, r)	\
if	(EXPECT((arg = assignarg(def, &rest, d)) != NULL)) { \
	EXPECT_STR(arg->name, n); \
	EXPECT_STR(arg->opt, o); \
	EXPECT_STR(arg->arg, a); \
	memfree(arg); \
} EXPECT_STR(rest, r)

	ASSIGNARG("name", NULL, "name", NULL, NULL, NULL);
	ASSIGNARG(" name  x ", NULL, "name", NULL, "x", NULL);
	ASSIGNARG(" name= x ", NULL, "name", NULL, " x ", NULL);
	ASSIGNARG(" \t\nname  x \n", "", "name  x \n", NULL, NULL, NULL);
	ASSIGNARG(" name  x ", " ", "name", NULL, NULL, "x ");
	ASSIGNARG("a[b]=c d", " \t", "a", "b", "c", "d");
	ASSIGNARG(" n=v \t, r", ", \t", "n", NULL, "v", " r");
	ASSIGNARG(" n=v \t,, r", ", \t", "n", NULL, "v", ", r");
	ASSIGNARG(" \t, r", ", \t", "", NULL, NULL, " r");
	ASSIGNARG("{ a {b}[(c)] d[{e}]=f g} \t\n, h", ", \t",
		"a {b}[(c)] d", "{e}", "f g", " h");
	ASSIGNARG("{ {a} b } c", ", \t", "a", NULL, NULL, "b } c");
	ASSIGNARG("{ [a] {b} } c", ", \t", "[a] {b} ", NULL, NULL, "c");
	ASSIGNARG("{ (a) [b]= } c", ", \t", "(a) ", "b", " ", "c");
	ASSIGNARG("n[x[y]=z]={a[b]=c}(d)[e]", " \t",
		"n", "x[y]=z", "{a[b]=c}(d)[e]", NULL);
	ASSIGNARG("n[x", "", "n[x", NULL, NULL, NULL);
}

TEST_SUITE
{
	TEST_RUN(argval);
	TEST_RUN(skiparg);
	TEST_RUN(assignarg);
}
