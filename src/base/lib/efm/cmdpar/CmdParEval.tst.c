#include <EFEU/UnitTest.h>
#include <EFEU/CmdPar.h>
#include <EFEU/mstring.h>
#include <EFEU/io.h>
#include <EFEU/procenv.h>
#include "utest/data.tst.h"

static CmdPar par;

static void pset(const char *name, const char *value)
{
	CmdPar_setval(&par, name, mstrcpy(value));
}

static const char *pget(const char *name)
{
	return CmdPar_getval(&par, name, NULL);
}

static int pcmd(const char *c, const char *v, const char *p, const char *a)
{
	CmdParEval *eval = CmdParEval_get(c);
	CmdParVar var = { NULL, NULL, NULL };
	return eval ? eval->func(&par, v ? CmdPar_var(&par, v, 1) : &var, p, a) : -1;
}

TEST_DECL(show)
{
	CmdParEval_show(out(), NULL);	// Für Testabdeckung //
	CmdParEval_show(out(), "$1 ");
	EXPECT_STR(data(), "append break config exit info insert lset "
	 	"manpage message set setlocale test usage vset ");
}

TEST_DECL(var)
{
	pset("a", NULL);
	EXPECT_TRUE(pcmd("append", "a", ":", "first") == 0);
	EXPECT_STR(pget("a"), "first");
	EXPECT_TRUE(pcmd("append", "a", ":", "second") == 0);
	EXPECT_STR(pget("a"), "first:second");

	pset("a", NULL);
	EXPECT_TRUE(pcmd("insert", "a", ":", "first") == 0);
	EXPECT_STR(pget("a"), "first");
	EXPECT_TRUE(pcmd("insert", "a", ":", "second") == 0);
	EXPECT_STR(pget("a"), "second:first");

	EXPECT_TRUE(pcmd("set", "a", "data", NULL) == 0);
	EXPECT_STR(pget("a"), "data");
	
	pset("x", "X\tV\"A\"L");
	pset("z", "ZV|A|L");
	pset("fmt", "x\\a\\| <|v\"a\"l|> <x> {} $0 {x} \"{x}\" {z} |{z}|");
	EXPECT_TRUE(pcmd("vset", "a", "fmt", "arg") == 0);
	EXPECT_STR(pget("a"), "x\\a| <|v\"a\"l|> <x> arg arg "
		"X\tV\"A\"L \"X\\tV\\\"A\\\"L\" "
		"ZV|A|L |ZV||A||L|");

	pset("fmt", NULL);
	EXPECT_TRUE(pcmd("vset", "a", "fmt", "arg") == 0);
	EXPECT_STR(pget("a"), NULL);

	EXPECT_TRUE(pcmd("lset", "a", ":XX:BAD {}:*:GOOD {}", "arg") == 0);
	EXPECT_STR(pget("a"), "GOOD arg");
}

TEST_DECL(ctrl)
{
	EXPECT_TRUE(pcmd("break", NULL, NULL, NULL) == CMDPAR_BREAK);

	EXPECT_STR(CurrentLocale, NULL);
	EXPECT_TRUE(pcmd("setlocale", NULL, "{}", "C") == 0);
	EXPECT_STR(CurrentLocale, "C");

	TEST_FORK
	{
		pcmd("exit", NULL, "42", NULL);
		TEST_EXIT; // not reached
	}

	CHILD_EXIT(42);
}

TEST_DECL(debug)
{
	TEST_DIR("debug");

	TEST_FORK
	{
		pset("var", "value");
		pset("x", "var");

		ioerr = io_fopen(UnitTest_tmpname("debug/test"), "w");
		EXPECT_TRUE(pcmd("test", "var", "{}", "arg") == 0);
		io_close(ioerr);

		ioerr = io_fopen(UnitTest_tmpname("debug/message"), "w");
		EXPECT_TRUE(pcmd("message", "var",
			"{var} {} {x} {{x}}", "arg") == 0);
		io_close(ioerr);

		AddInfo(NULL, "info", "Label", NULL, "Desc of $1:$2\n");
		iostd = io_fopen(UnitTest_tmpname("debug/info"), "w");
		EXPECT_TRUE(pcmd("info", NULL, "dump:info", NULL) == 0);
		io_close(iostd);
		TEST_EXIT;
	}

	CHILD_EXIT(0);
	TEST_CONTENT("debug/test",
		"var=\"value\", \"{}\", \"arg\" -> \"arg\"\n");
	TEST_CONTENT("debug/message", "value arg var value");
	TEST_CONTENT("debug/info", "@: Label\nDesc of info:Label\n");
}

TEST_DECL(doc)
{
	TEST_DIR("doc");

	TEST_FORK
	{
		// disable efeudoc
		setenv("PATH", UnitTest_tmpname(NULL), 1);
		pset(NULL, "CommandName");

		ioerr = io_fopen(UnitTest_tmpname("doc/usage"), "w");
		pcmd("usage", NULL, "@name", NULL);
		io_close(ioerr);

		iostd = io_fopen(UnitTest_tmpname("doc/man1"), "w");
		pcmd("manpage", NULL, NULL, "@name");
		io_close(iostd);

		pset(".manpage.fmt", "MANPAGE\n");
		pcmd("manpage", NULL, "{}", UnitTest_tmpname("doc/man2"));

		TEST_WAIT;
	}

	CHILD_EXIT(0);
	TEST_CONTENT("doc/usage", "CommandName -- CommandName\n");
	TEST_CONTENT("doc/man1", "CommandName -- CommandName\n");
	TEST_CONTENT("doc/man2", "MANPAGE\n");
}

static int f_test_add (CmdPar *cpar, CmdParVar *var,
	const char *par, const char *arg)
{
	return atoi(arg);
}

static CmdParEval test_add = { "test_add", "Testfunktion", f_test_add };

TEST_DECL(add)
{
	EXPECT_TRUE(pcmd("test_add", NULL, NULL, "99") == -1);
	CmdParEval_add(&test_add);
	EXPECT_TRUE(pcmd("test_add", NULL, NULL, "99") == 99);
}

TEST_SUITE
{
	CmdPar_init(&par);
	TEST_RUN(show);
	TEST_RUN(var);
	TEST_RUN(ctrl);
	TEST_RUN(debug);
	TEST_RUN(add);
	TEST_RUN(doc);
	CmdPar_clean(&par);
}
