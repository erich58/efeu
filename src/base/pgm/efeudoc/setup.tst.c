#include <EFEU/UnitTest.h>
#include "efeudoc.h"
#include <EFEU/utest/eval.tst.h>
#include <EFEU/utest/mstr.tst.h>

static Doc *doc = NULL;

static void doc_open()
{
	if	(!doc->out)
		doc->out = DocOut(doc->type, UnitTest_tmp("output")); 
}

static void doc_plain()
{
	if	(!doc->out)
		doc->out = DocOut(NULL, UnitTest_tmp("output")); 
}

static const char *doc_close()
{
	if	(doc->out)
	{
		io_close(doc->out);
		doc->out = NULL;
		return UnitTest_content("output");
	}

	return NULL;
}

static const char *copy(const char *expr)
{
	doc_open();
	Doc_scopy(doc, expr);
	return doc_close();
}

static const char *eval(const char *expr)
{
	doc_open();
	Doc_pushvar(doc);
	streval(expr);
	Doc_popvar(doc);
	return doc_close();
}

TEST_DECL(docvar)
{
	Doc_pushvar(doc);
	EXPECT_EVAL("var", "DocVar = {\n}");
	EXPECT_EVAL("load(\"int x = 42\", cout)", "42");
	EXPECT_EVAL("var", "DocVar = {\n\tx\n}");
	EXPECT_EVAL("x", "42");
	EXPECT_EVAL("(VarTab) this",
		"DocVar = {\n"
		"\tx\n"
		"}"
	);

	MATCH_EVAL("vtabstack(10, cout);",
		"DocVar = \\{\n"
		"\tx\n"
		"}, Doc\\[[0-9]*]\\(Doc), global\\[[0-9]*]\n"
	);

	EXPECT(doc->out == NULL);
	doc_open();
	EXPECT_EVAL("out", UnitTest_tpl("IO: @/output"));
	doc_close();
	EXPECT_EVAL("out", "NULL");

	EXPECT_INT(doc->stat, 0);
	EXPECT_EVAL("stat", "false");
	EXPECT_STR(eval("mode(0);"), "<@head>\n");
	EXPECT_INT(doc->stat, 1);
	EXPECT_EVAL("stat", "true");
	EXPECT_STR(eval("mode(0);"), "");

	doc_open();
	EXPECT_INT(doc->env.hmode, 0);
	EXPECT_EVAL("vmode", "true");
	EXPECT_EVAL("hmode", "false");
	EXPECT_EVAL("rmode", "false");
	EXPECT_EVAL("mode('h');", "");
	EXPECT_INT(doc->env.hmode, 1);
	EXPECT_EVAL("vmode", "false");
	EXPECT_EVAL("hmode", "true");
	EXPECT_EVAL("rmode", "false");
	TEST_EXPR(doc->env.hmode = 2);
	EXPECT_EVAL("vmode", "false");
	EXPECT_EVAL("hmode", "false");
	EXPECT_EVAL("rmode", "true");
	TEST_EXPR(doc->env.hmode = 1);
	EXPECT_EVAL("mode('v');", "");
	EXPECT_INT(doc->env.hmode, 0);
	PRINT_STR(doc_close());
	Doc_popvar(doc);
}

TEST_DECL(parse)
{
	Doc_pushvar(doc);
	EXPECT_EVAL("{ IO in = \" \t/*xyz*/   abc\";"
		"SkipSpace(in); copy(in, cout); }",
		"abc"
	);
	EXPECT_EVAL("{ IO in = \" \t//xyz\n   abc\";"
		"SkipWhite(in); copy(in, cout); }",
		"abc"
	);
	EXPECT_EVAL("ParseKey(\"@abc\", '@')", "true");
	EXPECT_EVAL("ParseKey(\"!abc\", '@')", "false");
	EXPECT_EVAL("ParseName(\"abc EOD\")", "\"abc\"");
	EXPECT_EVAL("ParseLine(\"abc 123\nEOD\")", "\"abc 123\"");
	EXPECT_EVAL("ParsePar(\"abc\n123\n\nEOD\")", "\"abc\\n123\\n\"");
	EXPECT_EVAL("ParseExpr(\"abc\nEOD\")", "\"abc\"");
	EXPECT_EVAL("ParseRegion(\"abc\n123\nend\nEOD\", \"end\")",
		"\"abc\\n123\\n\"");
	EXPECT_EVAL("ParseBlock(\"abc\n\\end\nEOD\", 1, NULL, \"end\", NULL)",
		"\"abc\\n\"");
	EXPECT_EVAL("ParseVerb(\"|abc| EOD\")", "\"abc\"");
	EXPECT_EVAL("ParseOpt(\"[abc] EOD\")", "\"abc\"");
	EXPECT_EVAL("ParseArg(\"{abc} EOD\")", "\"abc\"");

	EXPECT_EVAL("PlainArg()", "\"\"");
	EXPECT_EVAL("StdArg()", "\"<+copy><-copy>\"");
	EXPECT_EVAL("LongArg()", "NULL");
	EXPECT_EVAL("sync();sync();\n", "");
	doc->nl = 0;
	doc->env.cpos = 2;
	EXPECT_STR(eval("newline()"), "\n");
	EXPECT_INT(doc->env.cpos, 0);

	EXPECT_EVAL("expand(\"abc\")", "\"<+copy>abc<-copy>\"");
	EXPECT_EVAL("xexpand(\"abc\")", "\"abc\"");
	EXPECT_STR(eval("eval(NULL)"), "");
	Doc_popvar(doc);
}

TEST_DECL(cmd)
{
	doc->stat = 1;
	doc->env.hmode = 2;

	EXPECT_STR(eval("comment(\"Comment\")"), "<#Comment>\n");
	EXPECT_STR(eval("plain(\"Plain text\")"),
		"<+plain>\n"
		"Plain text<-plain>\n"
	);

	doc->env.cpos = 0;
	EXPECT_STR(eval("cmd('\n')"), "<!newline>");
	EXPECT_STR(eval("cmd('\f')"), "<!newpage>");
	EXPECT_STR(eval("cmd('a', \"name\")"), "<!appendix name>");
	EXPECT_STR(eval("cmd('c')"), "<!toc>");
	EXPECT_STR(eval("cmd('f')"), "<!lof>");
	EXPECT_STR(eval("cmd('t')"), "<!lot>");
	EXPECT_STR(eval("cmd('m', 42)"), "<!mark 42>");
	EXPECT_STR(eval("cmd('x')"), "");

	EXPECT_STR(eval("plaintex(\"expr\")"), "<!tex expr>");
	EXPECT_STR(eval("label('l', \"name\")"), "<!label name>");
	EXPECT_STR(eval("label('p', \"name\")"), "<!pageref name>");
	EXPECT_STR(eval("label('v', \"name\")"), "<!vref name>");
	EXPECT_STR(eval("label('m', \"name\")"), "<!manref name>");
	EXPECT_STR(eval("label('x', \"name\")"), "<!ref name>");

	doc_plain();
	EXPECT_STR(eval("label('m', \"name\")"), "name");
	doc_plain();
	EXPECT_STR(eval("label('x', \"name\")"), "[name]");

	EXPECT_STR(eval("index(\"name\", \"label\")"),
		"<!index name label>");
}

TEST_DECL(env)
{
	doc->stat = 1;
	doc->env.hmode = 2;

	EXPECT_STR(eval("url(\"name\", \"label\")"),
		"<+url \"name\">\n"
		"label<-url>\n"
	);

	EXPECT_STR(eval("newenv('c');endenv('c')"), "\n<+code>\n<-code>");
	EXPECT_STR(eval("newenv('h');endenv('h')"), "\n<+hang><-hang>");
	EXPECT_STR(eval("newenv('q');endenv('q')"), "\n<+quote><-quote>");
	EXPECT_STR(eval("newenv('i');endenv('i')"), "\n<+intro>\n<-intro>\n");
	EXPECT_STR(eval("newenv('s');endenv('s')"), "\n<+sloppy><-sloppy>");

	EXPECT_STR(eval("newenv('$');endenv('$')"), "<+f>\n<-f>\n");
	EXPECT_STR(eval("newenv('m', \"opt\", \"name\");endenv('m')"),
		"\n<+mpage \"opt\" \"name\">\n<-mpage>\n");
	EXPECT_STR(eval("newenv('p', \"opt\", \"name\");endenv('p')"),
		"\n<+spage \"opt\" \"name\">\n<-spage>\n");
	EXPECT_STR(eval("newenv('f', \"title\");endenv('f')"),
		"\n<+fig \"title\">\n<-fig>\n");
	EXPECT_STR(eval("newenv('t', \"title\");endenv('t')"),
		"\n<+table \"title\">\n<-table>\n");
	EXPECT_STR(eval("newenv('b', \"refname\");endenv('b')"),
		"\n<+bib \"refname\">\n<-bib>\n");
	EXPECT_STR(eval("newenv(0);endenv(0)"), "");

	EXPECT_STR(eval("endenv('a')"), "");
	EXPECT_STR(eval("endenv('x')"), "");
	EXPECT_STR(eval("endenv('l')"), "");

	EXPECT_STR(eval("input(\"verbatim\", \"Text\n\")"),
		"<#cmd=\"\" eval=0 cfg=0 base=verb alt=verb>\n"
		"<+verb>\n"
		"Text\n"
		"<-verb>\n"
		"<#EOF>\n"
	);
	EXPECT_STR(eval("include(\"verbatim\", \"/dev/null\")"),
		"<#include file: /dev/null:1>\n"
		"<#cmd=\"\" eval=0 cfg=0 base=verb alt=verb>\n"
		"<+verb>\n"
		"<-verb>\n"
		"<#EOF>\n"
	);
}

TEST_DECL(mac)
{
	TEST_FILE("test.dmac",
		"\\def\\rem	comment(ParseLine(cin));\n"
		"Description of @macro\n"
		"\\def@macro	macro content\n"
		"\\def\\tab	tab(ParseOpt(cin, true), ParseLine(cin));\n"
	);

	doc->stat = 1;
	doc->env.hmode = 2;

	Doc_pushvar(doc);
	EXPECT_EVAL("cfgname(\"/dev/null\")", "\"/dev/null\"");
	EXPECT_EVAL("config(\"test\")", "");
	EXPECT_EVAL("maclist()", "{\"@macro\", \"\\\\rem\", \"\\\\tab\"}");
	EXPECT_EVAL("testmac(\"@macro\")", "true");
	EXPECT_EVAL("showmac(\"@macro\")", "Description of @macro\n");
	Doc_popvar(doc);

	Doc_pushvar(doc);
	EXPECT(doc->cmd_stack->next == NULL);
	EXPECT_EVAL("push(\"local\")", "");
	EXPECT(doc->cmd_stack->next != NULL);
	EXPECT_EVAL("stack()", "{\"local\", \"global\"}");
	EXPECT_EVAL("def(\"@x=text\")", "");
	EXPECT_EVAL("maclist()", "{\"@x\"}");
	EXPECT_EVAL("pop()", "");
	EXPECT(doc->cmd_stack->next == NULL);
	Doc_popvar(doc);

	EXPECT_STR(copy("\\rem Comment\nText\n"), "<#Comment>\nText\n");
	EXPECT_STR(copy(
		"\\tab[opt] arg\n"
		"---\n"
		"a | b\n"
		"===\n"
		"1 | 2\n"
		"\n"
	),
		"\n"
		"<+tab option=opt coldef=arg>\n"
		"<!hline>\n"
		"<!tabbeg>\n"
		"a<!tabsep>\n"
		"b<!tabend>\n"
		"<!hline>\n"
		"<!tabbeg>\n"
		"1<!tabsep>\n"
		"2<!tabend>\n"
		"<-tab>\n"
	);
}

TEST_SUITE
{
	SetupDoc();
	CFGPath = mstrcpy(UnitTest_tmp(NULL));
	TEST_FILE("DocHead.cfg", "\n");

	doc = Doc_create("test");
	EXPECT_MSTR(rd_ident(doc),
		"type = \"test\", out = , stat = 0, indent = 0"
	);

	TEST_RUN(docvar);
	TEST_RUN(parse);
	TEST_RUN(cmd);
	TEST_RUN(env);
	TEST_RUN(mac);

	rd_deref(doc);
	CLEANUP_EVAL;
}
