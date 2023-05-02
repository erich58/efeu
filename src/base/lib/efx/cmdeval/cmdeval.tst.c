#include <EFEU/UnitTest.h>
#include <EFEU/cmdeval.h>
#include <EFEU/io.h>

TEST_DECL(CmdEval)
{
	UnitTest_stdout("stdout");
	TEST_FILE("cin",
		"str s = fgets(cin);Testdaten\n"
		"fprintf(cout, \"<%s>\n\", s);\n"
		"{}\n"
		"3*5\n"
		"!echo TEST\n"
		"3s\n"
	);
	IO *in = io_fopen(UnitTest_tmp("cin"), "r");
	IO *out = io_fopen(UnitTest_tmp("cout"), "w");
	CmdEval(in, out);
	io_close(in);
	io_close(out);
	UnitTest_stdout(NULL);
	EXPECT_CONTENT("cout",
		"<Testdaten>\n"
		"15\n"
		"3 \"Testdaten\"\n"
	);
	EXPECT_CONTENT("stdout", "TEST\n");
}

TEST_DECL(io_cmdeval)
{
	char buf[32];
	TEST_FILE("cin", "int a = 5; {} 3*5a\n$");
	IO *in = io_fopen(UnitTest_tmp("cin"), "r");
	in = io_cmdeval(in, "$");
	EXPECT_STR(io_gets(buf, sizeof buf, in), "15 5\n");
	EXPECT_STR(io_gets(buf, sizeof buf, in), NULL);
	PRINT_STR(rd_ident(in));
	io_close(in);

	TEST_FILE("cin", "{}");
	in = io_fopen(UnitTest_tmp("cin"), "r");
	in = io_cmdeval(in, NULL);
	EXPECT_STR(io_gets(buf, sizeof buf, in), NULL);
	io_close(in);
}

TEST_SUITE
{
	SetupStd();
	TEST_RUN(CmdEval);
	TEST_RUN(io_cmdeval);
}
