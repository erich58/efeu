/* UnitTest für CmdParEval und implizit für CmdPar_psub.
*/

#include <EFEU/UnitTest.h>
#include <EFEU/CmdPar.h>
#include <EFEU/mstring.h>
#include <EFEU/io.h>
#include <EFEU/procenv.h>
#include "utest/out.tst.h"
#include "utest/m2c.tst.h"

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
	CmdParEval_show(OUT(), NULL);	// Für Testabdeckung //
	MATCH_STR(DATA(), "^\\[append] .*\n\\[vset]");
	CmdParEval_show(OUT(), "$1 ");
	EXPECT_STR(DATA(), "append break config exit info insert lset "
	 	"manpage message set setlocale test usage vset ");
}

TEST_DECL(var)
{
	EXPECT_STR(M2C(CmdPar_psub(&par, NULL, "arg")), "arg");
	EXPECT_STR(M2C(CmdPar_psub(&par, "X = {}", "arg")), "X = arg");
	EXPECT_STR(M2C(CmdPar_psub(&par, "X = \"{}\"", "arg")), "X = \"arg\"");

	pset("a", NULL);
	EXPECT(pcmd("append", "a", ":", "first") == 0);
	EXPECT_STR(pget("a"), "first");
	EXPECT(pcmd("append", "a", ":", "second") == 0);
	EXPECT_STR(pget("a"), "first:second");

	pset("a", NULL);
	EXPECT(pcmd("insert", "a", ":", "first") == 0);
	EXPECT_STR(pget("a"), "first");
	EXPECT(pcmd("insert", "a", ":", "second") == 0);
	EXPECT_STR(pget("a"), "second:first");

	EXPECT(pcmd("set", "a", "data", NULL) == 0);
	EXPECT_STR(pget("a"), "data");
	
	pset("x", "X\tV\"A\"L");
	pset("z", "ZV|A|L");
	pset("fmt", "x\\a\\| <|v\"a\"l|> <x> {} $0 {x} \"{x}\" {z} |{z}|");
	EXPECT(pcmd("vset", "a", "fmt", "arg") == 0);
	EXPECT_STR(pget("a"), "x\\a| <|v\"a\"l|> <x> arg arg "
		"X\tV\"A\"L \"X\\tV\\\"A\\\"L\" "
		"ZV|A|L |ZV||A||L|");

	pset("fmt", NULL);
	EXPECT(pcmd("vset", "a", "fmt", "arg") == 0);
	EXPECT_STR(pget("a"), NULL);

	EXPECT(pcmd("lset", "a", ":XX:BAD {}:*:GOOD {}", "arg") == 0);
	EXPECT_STR(pget("a"), "GOOD arg");
}

TEST_DECL(ctrl)
{
	EXPECT(pcmd("break", NULL, NULL, NULL) == CMDPAR_BREAK);

	EXPECT_STR(CurrentLocale, NULL);
	EXPECT(pcmd("setlocale", NULL, "{}", "C") == 0);
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
		EXPECT(pcmd("test", "var", "{}", "arg") == 0);
		io_close(ioerr);

		ioerr = io_fopen(UnitTest_tmpname("debug/message"), "w");
		EXPECT(pcmd("message", "var",
			"{var} {} {x} {{x}}", "arg") == 0);
		io_close(ioerr);

		AddInfo(NULL, "info", "Label", NULL, "Desc of $1:$2\n");
		iostd = io_fopen(UnitTest_tmpname("debug/info"), "w");
		EXPECT(pcmd("info", NULL, "dump:info", NULL) == 0);
		io_close(iostd);
		TEST_EXIT;
	}

	CHILD_EXIT(0);
	EXPECT_CONTENT("debug/test",
		"var=\"value\", \"{}\", \"arg\" -> \"arg\"\n");
	EXPECT_CONTENT("debug/message", "value arg var value");
	EXPECT_CONTENT("debug/info", "@: Label\nDesc of info:Label\n");
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

		iostd = io_fopen(UnitTest_tmpname("doc/man3"), "w");
		pcmd("manpage", NULL, "arg", "@xxx $$\n");
		io_close(iostd);

		TEST_EXIT;
	}

	CHILD_EXIT(0);
	EXPECT_CONTENT("doc/usage", "CommandName -- CommandName\n");
	EXPECT_CONTENT("doc/man1", "CommandName -- CommandName\n");
	EXPECT_CONTENT("doc/man2", "MANPAGE\n");
	EXPECT_CONTENT("doc/man3", "@xxx $\n");
}

TEST_DECL(config)
{
	TEST_FILE("config.cnf", "A = V1\nB = V2\n");
	EXPECT(pcmd("config", NULL, "{}",
		UnitTest_tmpname("config.cnf")) == 0);
	EXPECT_STR(pget("A"), "V1");
	EXPECT_STR(pget("B"), "V2");

	TEST_FORK
	{
		freopen(UnitTest_tmpname("config.err"), "w", stderr);
		pcmd("config", NULL, "{}", UnitTest_tmpname("undefined.cnf"));
		TEST_EXIT;
	}

	CHILD_EXIT(0);
	MATCH_CONTENT("config.err",
		"application file.*undefined.cnf.*not found");
}

static int f_test_add (CmdPar *cpar, CmdParVar *var,
	const char *par, const char *arg)
{
	return atoi(arg);
}

static CmdParEval test_add = { "test_add", "Testfunktion", f_test_add };

TEST_DECL(add)
{
	EXPECT(pcmd("test_add", NULL, NULL, "99") == -1);
	CmdParEval_add(&test_add);
	EXPECT(pcmd("test_add", NULL, NULL, "99") == 99);
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
	TEST_RUN(config);
	CmdPar_clean(&par);
}
