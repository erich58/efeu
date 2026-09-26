#include <EFEU/UnitTest.h>
#include <EFEU/preproc.h>
#include <EFEU/ioctrl.h>
#include <EFEU/utest/in.tst.h>
#include <EFEU/utest/out.tst.h>

static const char *pp(const char *input)
{
	PushMacroTab(NULL);
	IO *in = io_cmdpreproc(io_refer(IN(input)));
	io_copy(in, OUT());
	io_close(in);
	rd_deref(PopMacroTab());
	return input;
}

#define	PPINPUT(input)	\
UnitTest_strinfo(__FILE__, __LINE__, "INFO", pp(input), "input")

TEST_DECL(pp_ctrl)
{
	IO *in = io_cmdpreproc(io_cstr("/*XXX*/a"));
	EXPECT(io_ctrl(in, IO_LINE) == 1);
	EXPECT(io_ctrl(in, IOPP_EXPAND, 0) == 1);
	EXPECT(io_ctrl(in, IOPP_EXPAND, 0) == 0);

	EXPECT(io_getc(in) == 'a');
	char *buf = NULL;
	EXPECT(io_ctrl(in, IOPP_COMMENT, &buf) == 0);
	EXPECT_STR(buf, "XXX");
	EXPECT(io_rewind(in) == EOF);
	io_close(in);
}

TEST_DECL(pp_id)
{
	TEST_FILE("input",
		"start\n"
		"__ID__ line 2\n"
		"__FILE__ __LINE__ line 3\n"
		"end\n"
	);

	TEST_FORK
	{
		chdir(UnitTest_tmp(NULL));
		IO *in = io_cmdpreproc(io_fopen("input", "r"));
		io_copy(in, OUT());
		io_close(in);
		EXPECT_OUT(
			"start\n"
			"\"input:2\" line 2\n"
			"\"input\" 3 line 3\n"
			"end\n"
		);
		TEST_EXIT;
	}

	CHILD_EXIT(0);
}

TEST_DECL(pp_eat)
{
	PPINPUT(" 'a' \"b\" \\x\n");
	EXPECT_OUT(" 'a' \"b\" \\x\n");

	PPINPUT(" ##a\n");
	EXPECT_OUT(" #a\n");

	PPINPUT("#!abc\\\nx \n");
	EXPECT_OUT("");

	PPINPUT(" \t#!abc \n");
	EXPECT_OUT(" \t\n");

	PPINPUT("abc.\\\n \txyz\n");
	EXPECT_OUT("abc.xyz\n");

	UnitTest_stderr("err");
	PPINPUT("\r\n");
	EXPECT_OUT("\n");
	UnitTest_stderr(NULL);
	EXPECT_CONTENT("err", "MSG 199: CR detected\n");
}

TEST_DECL(pp_get)
{
	PPINPUT("#define x\nx");
	EXPECT_OUT("");
	PPINPUT("#define x a\nx");
	EXPECT_OUT("a");
}

TEST_DECL(if0)
{
	PPINPUT(
		"start\n"
		"# if 0\n"
		"body\n"
		"##marked\n"
		"#endif\n"
		"end\n"
	);
	EXPECT_OUT(
		"start\n"
		"end\n"
	);

	UnitTest_stderr("err");
	PPINPUT(
		"start\n"
		"#if 0\n"
		"body\n"
	);
	EXPECT_OUT(
		"start\n"
	);
	UnitTest_stderr(NULL);
	EXPECT_CONTENT("err", "ERR 196: EOF in test block (start line 2)\n");
}

TEST_DECL(unknown)
{

	UnitTest_stderr("err");
	PPINPUT(
		"start\n"
		"# not_a_directive some data\n"
		"end\n"
	);
	EXPECT_OUT(
		"start\n"
		"end\n"
	);
	UnitTest_stderr(NULL);
	EXPECT_CONTENT("err", "ERR 191: unknown directive not_a_directive\n");
}

TEST_DECL(error)
{
	UnitTest_stderr("err");
	PPINPUT(
		"start\n"
		"#error ERROR NOTE\n"
		"end\n"
	);
	EXPECT_OUT(
		"start\n"
		"end\n"
	);
	UnitTest_stderr(NULL);
	EXPECT_CONTENT("err",
		"ERROR NOTE\n"
	);
}

TEST_DECL(block)
{
	PPINPUT(
		"start\n"
		"#if 0\n"
		"  ignored 1\n"
		"  #if 1\n"
		"    ignored 2\n"
		"    #if 1\n"
		"      ignored 2.1\n"
		"    #endif\n"
		"    #ifdef x\n"
		"      ignored 2.2\n"
		"    #endif\n"
		"    #ifndef x\n"
		"      ignored 2.3\n"
		"    #endif\n"
		"    #error ignored\n"
		"  #endif\n"
		"  #ifdef x\n"
		"    ignored 3\n"
		"  #endif\n"
		"  #ifndef x\n"
		"    ignored 4\n"
		"  #endif\n"
		"  #error ignored\n"
		"#elif 0\n"
		"  ignored\n"
		"#elif 1\n"
		"  used 1\n"
		"  #ifdef x\n"
		"    ignored\n"
		"  #else x\n"
		"    x undefined\n"
		"    #ifndef y\n"
		"      y undefined\n"
		"    #endif\n"
		"  #endif\n"
		"#elif 1\n"
		"  ignored\n"
		"#else\n"
		"  ignored\n"
		"#endif\n"
		"end\n"
	);
	EXPECT_OUT(
		"start\n"
		"  used 1\n"
		"    x undefined\n"
		"      y undefined\n"
		"end\n"
	);
}

TEST_DECL(block_error)
{
	UnitTest_stderr("err");
	PPINPUT(
		"#elif 1\n"
		"#else\n"
		"#endif\n"
	);
	UnitTest_stderr(NULL);
	EXPECT_CONTENT("err",
		"ERR 193: ELIF unexpected.\n"
		"ERR 194: ELSE unexpected.\n"
		"ERR 195: ENDIF unexpected.\n"
	);

	UnitTest_stderr("err");
	PPINPUT(
		"#if 1\n"
		"#else\n"
		"#elif 1\n"
		"#endif\n"
	);
	UnitTest_stderr(NULL);
	EXPECT_CONTENT("err",
		"ERR 193: ELIF unexpected.\n"
	);

	UnitTest_stderr("err");
	PPINPUT(
		"#if 1\n"
		"#else\n"
		"#else\n"
		"#endif\n"
	);
	UnitTest_stderr(NULL);
	EXPECT_CONTENT("err",
		"ERR 194: ELSE unexpected.\n"
	);
}

TEST_DECL(include)
{
	TEST_FILE("sub",
		"sub 1\n"
		"#return\n"
		"sub 2\n"
	);
	PPINPUT(
		"main 1\n"
		"#include <sub>\n"
		"main 2\n"
	);
	EXPECT_OUT(
		"main 1\n"
		"sub 1\n"
		"main 2\n"
	);

	TEST_FILE("h1", "header 1\n");
	PPINPUT( "#include \"h1\"\n");
	EXPECT_OUT("header 1\n");

	PPINPUT("#include <h1>\n");
	EXPECT_OUT("header 1\n");

	PPINPUT("#include \"h1\"\n");
	EXPECT_OUT("header 1\n");

	PPINPUT("#include sprintf(\"<h%d>\", 1)\n");
	EXPECT_OUT("header 1\n");

	UnitTest_stderr("err");
	PPINPUT("#include <h2>\n");
	UnitTest_stderr(NULL);
	EXPECT_CONTENT("err",
		"ERR 197: File h2 not found.\n"
	);

	UnitTest_stderr("err");
	PPINPUT("#include\n");
	UnitTest_stderr(NULL);
	EXPECT_CONTENT("err",
		"ERR 198: Missing file name.\n"
	);
}

TEST_SUITE
{
	SetupStd();
	SetApplPath(UnitTest_tmp(NULL));
	memfree(IncPath);
	IncPath = mstrcpy(UnitTest_tmp(NULL));
	TEST_FILE("preproc.msg", 
		"191	191: unknown directive $1\n"
		"193	193: ELIF unexpected.\n"
		"194	194: ELSE unexpected.\n"
		"195	195: ENDIF unexpected.\n"
		"196	196: EOF in test block (start line $1)\n"
		"197	197: File $1 not found.\n"
		"198	198: Missing file name.\n"
		"199	199: CR detected\n"
	);
	TEST_RUN(pp_ctrl);
	TEST_RUN(pp_id);
	TEST_RUN(pp_eat);
	TEST_RUN(pp_get);
	TEST_RUN(if0);
	TEST_RUN(unknown);
	TEST_RUN(error);
	TEST_RUN(block);
	TEST_RUN(block_error);
	TEST_RUN(include);
	CLEANUP_IN;
	CLEANUP_OUT;
}
