#include <EFEU/UnitTest.h>
#include <EFEU/MakeDepend.h>
#include "efeudoc.h"
#include <EFEU/utest/mstr.tst.h>

extern char *CFGPath;

static IO *test_filter (IO *io)
{
	io_puts("filter added\n", io);
	return io;
}

static IO *test_open (const char *name, const char *par)
{
	IO *io = io_strbuf(NULL);
	io_printf(io, "name = %s\npar = %s\n", name, par);
	return io;
}

static DocType plain = {
	"Name", "Desc", NULL, NULL, NULL
};

static DocType extended = {
	"Name", "Desc", test_filter, test_open, "Parameter"
};


TEST_DECL(setup)
{
	SetupStd();
	UnitTest_mkcmd("tex2ps", "#!/bin/sh\necho tex2ps $*\n");
	CFGPath = mstrpaste(":", TOP "/lib/efeudoc", UnitTest_tmp(NULL));
	PRINT_STR(CFGPath);
	DocOutInfo("DocOut", "document drivers");
}

TEST_DECL(GetDocType)
{
	EXPECT(GetDocType(NULL) == NULL);

	TEST_FORK
	{
		UnitTest_stdout("stdout");
		EXPECT(GetDocType("?") == NULL);
		TEST_EXIT;
	}

	CHILD_EXIT(0);
	EXPECT_CONTENT("stdout",
		"test\ttest output\n"
		"term\tterminal output\n"
		"LaTeX\tLaTeX source\n"
		"tex2ps\tPostScript over LaTeX\n"
		"tex2pdf\tPDF over LaTeX\n"
		"man\tman source\n"
		"nroff\tnroff formatted manpage\n"
		"html\tHTML output\n"
	);

	TEST_FORK
	{
		UnitTest_stderr("stderr");
		EXPECT(GetDocType("unknown_doc_type") == NULL);
		TEST_EXIT;
	}

	CHILD_EXIT(1);
	EXPECT_CONTENT("stderr",
		"ERR !: unknown document type unknown_doc_type.\n"
		"test\ttest output\n"
		"term\tterminal output\n"
		"LaTeX\tLaTeX source\n"
		"tex2ps\tPostScript over LaTeX\n"
		"tex2pdf\tPDF over LaTeX\n"
		"man\tman source\n"
		"nroff\tnroff formatted manpage\n"
		"html\tHTML output\n"
	);
}

TEST_DECL(DocOut)
{
	IO *io;
	EXPECT_PTR(io = DocOut(NULL, NULL));
	EXPECT_MSTR(rd_ident(io), "<stdout>");
	io_close(io);

	TEST_FILE("input", "");
	EXPECT_PTR(io = DocOut(&plain, UnitTest_tmp("input")));
	EXPECT_MSTR(rd_ident(io), UnitTest_tmp("input"));
	io_close(io);

	EXPECT_PTR(io = DocOut(&extended, "Input"));
	io_rewind(io);
	EXPECT_MSTR(io_mgets(io, NULL),
		"name = Input\n"
		"par = Parameter\n"
		"filter added\n"
	);
	io_close(io);

	io = io_fileopen(UnitTest_tmp("output"), "w");
	io = DocFilter(&extended, io);
	io_close(io);
	EXPECT_CONTENT("output", "filter added\n");
}

TEST_DECL(post_open)
{
	IO *io;

	UnitTest_stdout("stdout");
	EXPECT_PTR(io = DocOut(GetDocType("tex2ps"), NULL));
	EXPECT_MSTR(rd_ident(io), "LaTeX(latin9(<!tex2ps>))");
	io_close(io);
	UnitTest_stdout(NULL);
	EXPECT_CONTENT("stdout", "tex2ps -n3 -\n");

	EXPECT_PTR(io = DocOut(GetDocType("tex2ps"), UnitTest_tmp("output")));
	EXPECT_MSTR(rd_ident(io), "LaTeX(latin9(<!tex2ps>))");
	io_close(io);
	EXPECT_CONTENT("output", "tex2ps -n3 -\n");

	MakeDepend = 1;
	EXPECT_PTR(io = DocOut(GetDocType("tex2ps"), NULL));
	EXPECT_MSTR(rd_ident(io), "LaTeX()");
	io_close(io);
	MakeDepend = 0;
}

TEST_SUITE
{
	TEST_RUN(setup);
	TEST_RUN(GetDocType);
	TEST_RUN(DocOut);
	TEST_RUN(post_open);
	CLEANUP_MSTR;

}
