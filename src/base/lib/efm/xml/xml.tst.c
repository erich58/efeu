#include <EFEU/UnitTest.h>
#include <EFEU/strbuf.h>
#include <EFEU/xml.h>

#include <EFEU/utest/in.tst.h>

static STRBUF(buf_print, 64);
static STRBUF(buf_compact, 64);
static STRBUF(buf_tlist, 64);
static STRBUF(buf_dump, 64);

static const char *buf_data(StrBuf *buf)
{
	const char *p = sb_nul(buf);
	sb_trunc(buf);
	return p;
}

static int x_put(int c, void *par)
{
	return sb_put(c, par);
}

static void x_close(void *par)
{
	sb_puts("*CLOSE*\n", &buf_print);
}

static XMLOutput out_print = { x_put, &buf_print };
static XMLOutput out_compact = { x_put, &buf_compact };
static XMLOutput out_tlist = { x_put, &buf_tlist };
static XMLOutput out_dump = { x_put, &buf_dump };

static unsigned test_limit = 0;

static void *test_action(XMLBuf *xml, const char *name,
	const char *data, void *par)
{
	xml_print(xml, name, data, &out_print);
	xml_compact(xml, name, data, &out_compact);
	xml_tlist(xml, name, data, &out_tlist);
	xml_dump(xml, name, data, &out_dump);

	if	(test_limit > 1)
	{
		--test_limit;
		return NULL;
	}

	sb_puts("*BREAK*\n", &buf_dump);
	return "BREAK";
}

static const char *parse(unsigned lim, const char *def)
{
	test_limit = lim;
	XMLBuf *x = xml_create(NULL, test_action, NULL);
	return XMLBuf_parse(x, IN(def));
}

#define	PARSE(def)	\
	UnitTest_strinfo(__FILE__, __LINE__, "?PARSE", def, "input"); \
	UnitTest_strcheck(__FILE__, __LINE__, "==", parse(-1, def), \
		NULL, "parse(-1, input)", "NULL")

#define	XPARSE(lim, def)	\
	UnitTest_strinfo(__FILE__, __LINE__, "?PARSE", def, "input"); \
	UnitTest_strcheck(__FILE__, __LINE__, "==", parse(lim, def), \
		"BREAK", "parse(" # lim ", input)", "break")

#define	EXPECT_BUF(a, b) \
	UnitTest_strcheck(__FILE__, __LINE__, "==", buf_data(&a), b, # a, # b)

#define	LMATCH_BUF(a, b) \
	UnitTest_linematch(__FILE__, __LINE__, buf_data(&a), b, # a)

TEST_DECL(create)
{
	XMLBuf *x;
	EXPECT_PTR(x = xml_create(NULL, NULL, (void *) 0xf00));
	XMLBuf_atclose(x, x_close);
	EXPECT(XMLBuf_close(x) == NULL);
	EXPECT_BUF(buf_print, "*CLOSE*\n");

	EXPECT_PTR(x = xml_create(NULL, NULL, (void *) 0xf00));
	EXPECT(XMLBuf_close(x) == (void *) 0xf00);

	EXPECT_PTR(x = xml_create(NULL, test_action, NULL));
	XMLBuf_dtd(x, "html public");
	XMLBuf_pi(x, "xml", "version=\"1.0\" encoding=\"utf-8\"");
	XMLBuf_pi(x, "extra", " ");
	XMLBuf_close(x);
	EXPECT_BUF(buf_print,
		"<!DOCTYPE html public>\n"
		"<?xml version=\"1.0\" encoding=\"utf-8\"?>\n"
		"<?extra?>\n"
	);
	EXPECT_BUF(buf_compact,
		"<!DOCTYPE html public>\n"
		"<?xml version=\"1.0\" encoding=\"utf-8\"?>\n"
		"<?extra?>\n"
	);
	EXPECT_BUF(buf_tlist, "");
	EXPECT_BUF(buf_dump,
		"dtd    0 0 path=\"\" name=\"html public\" data=\"\"\n"
		"pi     0 0 path=\"\" name=\"xml\" data=\"version=\\\"1.0\\\" encoding=\\\"utf-8\\\"\"\n"
		"pi     0 0 path=\"\" name=\"extra\" data=\"\"\n"
	);

	EXPECT_PTR(x = xml_create(NULL, test_action, NULL));
	XMLBuf_beg(x, "root");
	XMLBuf_close(x);
	EXPECT_BUF(buf_print, "<root/>\n");
	EXPECT_BUF(buf_compact, "<root/>");
	EXPECT_BUF(buf_tlist,
		"+root\n"
		"/root\n"
	);
	EXPECT_BUF(buf_dump,
		"beg    0 0 path=\"/root\" name=\"root\" data=\"\"\n"
		"end    0 1 path=\"/root\" name=\"root\" data=\"\"\n"
	);

	EXPECT_PTR(x = xml_create(NULL, test_action, NULL));
	XMLBuf_beg(x, "root");
	XMLBuf_entry(x, "tag", "value");
	XMLBuf_err(x, "fmt %s", "arg");
	XMLBuf_data(x, xml_comm, "comment");
	XMLBuf_end(x);
	XMLBuf_close(x);
	EXPECT_BUF(buf_print,
		"<root>\n"
		"  <tag>\n"
		"    value\n"
		"  </tag>\n"
		"  <!--\n"
		"ERROR: fmt arg\n"
		"  -->\n"
		"  <!--comment-->\n"
		"</root>\n"
	);
	EXPECT_BUF(buf_compact, "<root><tag>value</tag></root>");
	EXPECT_BUF(buf_tlist,
		"+root\n"
		"+tag\n"
		"\tvalue\n"
		"/tag\n"
		"/root\n"
	);
	EXPECT_BUF(buf_dump,
		"beg    0 0 path=\"/root\" name=\"root\" data=\"\"\n"
		"beg    1 1 path=\"/root/tag\" name=\"tag\" data=\"\"\n"
		"data   2 1 path=\"/root/tag\" name=\"tag\" data=\"value\"\n"
		"end    1 0 path=\"/root/tag\" name=\"tag\" data=\"\"\n"
		"err    1 0 path=\"/root\" name=\"root\" data=\"fmt arg\"\n"
		"comm   1 0 path=\"/root\" name=\"root\" data=\"comment\"\n"
		"end    0 0 path=\"/root\" name=\"root\" data=\"\"\n"
	);

	EXPECT_PTR(x = xml_create(NULL, test_action, NULL));
	XMLBuf_beg(x, "root");
	XMLBuf_beg(x, "tag");
	XMLBuf_att(x, "opt", "x<&>\"");
	XMLBuf_data(x, xml_data, "<&>");
	XMLBuf_fmt(x, xml_data, "%d", 42);
	XMLBuf_end(x);
	XMLBuf_att(x, "bad", "option");
	XMLBuf_close(x);
	EXPECT_BUF(buf_print,
		"<root>\n"
		"  <tag opt=\"x&lt;&amp;&gt;&quot;\">\n"
		"    &lt;&amp;&gt;\n"
		"    42\n"
		"  </tag>\n"
		"  <!--\n"
		"ERROR: unexpected attribut bad='option'\n"
		"  -->\n"
		"</root>\n"
	);
	EXPECT_BUF(buf_compact,
		"<root>"
		"<tag opt=\"x&lt;&amp;&gt;&quot;\">"
		"&lt;&amp;&gt;"
		"42"
		"</tag>"
		"</root>"
	);
	EXPECT_BUF(buf_tlist,
		"+root\n"
		"+tag\n"
		"\t<&>\n"
		"\t42\n"
		"/tag\n"
		"/root\n"
	);
	EXPECT_BUF(buf_dump,
		"beg    0 0 path=\"/root\" name=\"root\" data=\"\"\n"
		"beg    1 1 path=\"/root/tag\" name=\"tag\" data=\"\"\n"
		"att    2 1 path=\"/root/tag\" name=\"opt\" data=\"x<&>\\\"\"\n"
		"data   2 1 path=\"/root/tag\" name=\"tag\" data=\"<&>\"\n"
		"data   2 0 path=\"/root/tag\" name=\"tag\" data=\"42\"\n"
		"end    1 0 path=\"/root/tag\" name=\"tag\" data=\"\"\n"
		"err    1 0 path=\"/root\" name=\"root\" data=\"unexpected attribut bad='option'\"\n"
		"end    0 0 path=\"/root\" name=\"root\" data=\"\"\n"
	);

	EXPECT_PTR(x = xml_create(NULL, test_action, NULL));
	XMLBuf_beg(x, "root");
	XMLBuf_beg(x, "tag1");
	XMLBuf_data(x, xml_data, NULL);
	XMLBuf_end(x);
	XMLBuf_beg(x, "tag2");
	XMLBuf_data(x, xml_cdata, "a<&>b\t\n\\x");
	XMLBuf_close(x);
	EXPECT_BUF(buf_print,
		"<root>\n"
		"  <tag1>\n"
		"  </tag1>\n"
		"  <tag2>\n"
		"    <![CDATA[a<&>b\t\n\\x]]>\n"
		"  </tag2>\n"
		"</root>\n"
	);
	EXPECT_BUF(buf_compact,
		"<root>"
		"<tag1>"
		"</tag1>"
		"<tag2>"
		"<![CDATA[a<&>b\t\n\\x]]>"
		"</tag2>"
		"</root>"
	);
	EXPECT_BUF(buf_tlist,
		"+root\n"
		"+tag1\n"
		"\t\n"
		"/tag1\n"
		"+tag2\n"
		"\ta<&>b\t\n\\x\n"
		"/tag2\n"
		"/root\n"
	);
	EXPECT_BUF(buf_dump,
		"beg    0 0 path=\"/root\" name=\"root\" data=\"\"\n"
		"beg    1 1 path=\"/root/tag1\" name=\"tag1\" data=\"\"\n"
		"data   2 1 path=\"/root/tag1\" name=\"tag1\" data=\"\"\n"
		"end    1 0 path=\"/root/tag1\" name=\"tag1\" data=\"\"\n"
		"beg    1 0 path=\"/root/tag2\" name=\"tag2\" data=\"\"\n"
		"cdata  2 1 path=\"/root/tag2\" name=\"tag2\" data=\"a<&>b\\t\\n\\\\x\"\n"
		"end    1 0 path=\"/root/tag2\" name=\"tag2\" data=\"\"\n"
		"end    0 0 path=\"/root\" name=\"root\" data=\"\"\n"
	);
}

TEST_DECL(main)
{
	PARSE(NULL);
	EXPECT_BUF(buf_dump, "");
	EXPECT_BUF(buf_print, "");
	EXPECT_BUF(buf_compact, "");
	EXPECT_BUF(buf_tlist, "");

	PARSE("");
	EXPECT_BUF(buf_dump, "");
	EXPECT_BUF(buf_print, "");
	EXPECT_BUF(buf_compact, "");
	EXPECT_BUF(buf_tlist, "");

	PARSE(" \t\n");
	EXPECT_BUF(buf_dump, "");
	EXPECT_BUF(buf_print, "");
	EXPECT_BUF(buf_compact, "");
	EXPECT_BUF(buf_tlist, "");

	PARSE("x\n");
	EXPECT_BUF(buf_dump,
		"err    0 0 path=\"\" name=\"\" "
		"data=\"unexpected character 'x'\"\n"
	);
	EXPECT_BUF(buf_print,
		"<!--\n"
		"ERROR: unexpected character 'x'\n"
		"-->\n"
	);
	EXPECT_BUF(buf_compact, "");
	EXPECT_BUF(buf_tlist, "");

	PARSE("</\n");
	EXPECT_BUF(buf_dump,
		"err    0 0 path=\"\" name=\"\" "
		"data=\"unexpected character sequence '</'\"\n"
	);
	EXPECT_BUF(buf_print,
		"<!--\n"
		"ERROR: unexpected character sequence '</'\n"
		"-->\n"
	);
	EXPECT_BUF(buf_compact, "");
	EXPECT_BUF(buf_tlist, "");

	PARSE("<x>\n");
	EXPECT_BUF(buf_dump,
		"beg    0 0 path=\"/x\" name=\"x\" data=\"\"\n"
		"err    1 1 path=\"/x\" name=\"x\" "
			"data=\"unexpected end of file\"\n"
	);
	EXPECT_BUF(buf_print,
		"<x>\n"
		"  <!--\n"
		"ERROR: unexpected end of file\n"
		"  -->\n"
	);
	EXPECT_BUF(buf_compact, "<x>");
	EXPECT_BUF(buf_tlist, "+x\n");

	PARSE("<x>xdata<y>ydata</y></x>\n");
	EXPECT_BUF(buf_dump,
		"beg    0 0 path=\"/x\" name=\"x\" data=\"\"\n"
		"data   1 1 path=\"/x\" name=\"x\" data=\"xdata\"\n"
		"beg    1 0 path=\"/x/y\" name=\"y\" data=\"\"\n"
		"data   2 1 path=\"/x/y\" name=\"y\" data=\"ydata\"\n"
		"end    1 0 path=\"/x/y\" name=\"y\" data=\"\"\n"
		"end    0 0 path=\"/x\" name=\"x\" data=\"\"\n"
	);
	EXPECT_BUF(buf_print,
		"<x>\n"
		"  xdata\n"
		"  <y>\n"
		"    ydata\n"
		"  </y>\n"
		"</x>\n"
	);
	EXPECT_BUF(buf_compact, "<x>xdata<y>ydata</y></x>");
	EXPECT_BUF(buf_tlist,
		"+x\n"
		"	xdata\n"
		"+y\n"
		"	ydata\n"
		"/y\n"
		"/x\n"
	);

	XPARSE(4, "<x>xdata<y>ydata</y></x>\n");
	EXPECT_BUF(buf_dump,
		"beg    0 0 path=\"/x\" name=\"x\" data=\"\"\n"
		"data   1 1 path=\"/x\" name=\"x\" data=\"xdata\"\n"
		"beg    1 0 path=\"/x/y\" name=\"y\" data=\"\"\n"
		"data   2 1 path=\"/x/y\" name=\"y\" data=\"ydata\"\n"
		"*BREAK*\n"
	);
	EXPECT_BUF(buf_print,
		"<x>\n"
		"  xdata\n"
		"  <y>\n"
		"    ydata\n"
	);
	EXPECT_BUF(buf_compact, "<x>xdata<y>ydata");
	EXPECT_BUF(buf_tlist,
		"+x\n"
		"	xdata\n"
		"+y\n"
		"	ydata\n"
	);
}

TEST_DECL(doc)
{
	PARSE(
		"<?xml version=\"1.0\"?>\n"
		"<!doctype html>\n"
		"<root>"
		"&amp;&euro;&#xC4;&#64;data"
		"</root>\n"
	);
	EXPECT_BUF(buf_dump,
		"pi     0 0 path=\"\" name=\"xml\" "
			"data=\"version=\\\"1.0\\\"\"\n"
		"dtd    0 0 path=\"\" name=\"html\" data=\"\"\n"
		"beg    0 0 path=\"/root\" name=\"root\" data=\"\"\n"
		"data   1 1 path=\"/root\" name=\"root\" "
			"data=\"&\342\202\254\303\204@data\"\n"
		"end    0 0 path=\"/root\" name=\"root\" data=\"\"\n"
	);
	EXPECT_BUF(buf_print,
		"<?xml version=\"1.0\"?>\n"
		"<!DOCTYPE html>\n"
		"<root>\n"
		"  &amp;\342\202\254\303\204@data\n"
		"</root>\n"
	);
	EXPECT_BUF(buf_compact,
		"<?xml version=\"1.0\"?>\n"
		"<!DOCTYPE html>\n"
		"<root>"
		"&amp;\342\202\254\303\204@data"
		"</root>"
	);
	EXPECT_BUF(buf_tlist,
		"+root\n"
		"\t&\342\202\254\303\204@data\n"
		"/root\n"
	);
}

TEST_DECL(instruction)
{
	PARSE("<?");
	EXPECT_BUF(buf_dump,
		"err    0 0 path=\"\" name=\"\" "
		"data=\"EOF in process instruction: ''\"\n"
	);
	EXPECT_BUF(buf_print,
		"<!--\n"
		"ERROR: EOF in process instruction: ''\n"
		"-->\n"
	);
	EXPECT_BUF(buf_compact, "");
	EXPECT_BUF(buf_tlist, "");

	PARSE("<?data");
	EXPECT_BUF(buf_dump,
		"err    0 0 path=\"\" name=\"\" "
		"data=\"EOF in process instruction: 'data'\"\n"
	);
	EXPECT_BUF(buf_print,
		"<!--\n"
		"ERROR: EOF in process instruction: 'data'\n"
		"-->\n"
	);
	EXPECT_BUF(buf_compact, "");
	EXPECT_BUF(buf_tlist, "");

	PARSE("<?name data");
	EXPECT_BUF(buf_dump,
		"err    0 0 path=\"\" name=\"name\" "
		"data=\"EOF in process instruction: 'data'\"\n"
	);
	EXPECT_BUF(buf_print,
		"<!--\n"
		"ERROR: EOF in process instruction: 'data'\n"
		"-->\n"
	);
	EXPECT_BUF(buf_compact, "");
	EXPECT_BUF(buf_tlist, "");

	PARSE("<?xml?>");
	EXPECT_BUF(buf_dump,
		"pi     0 0 path=\"\" name=\"xml\" "
		"data=\"\"\n"
	);
	EXPECT_BUF(buf_print, "<?xml?>\n");
	EXPECT_BUF(buf_compact, "<?xml?>\n");
	EXPECT_BUF(buf_tlist, "");

	PARSE("<?xml version=\"1.0\"?>");
	EXPECT_BUF(buf_dump,
		"pi     0 0 path=\"\" name=\"xml\" "
		"data=\"version=\\\"1.0\\\"\"\n"
	);
	EXPECT_BUF(buf_print, "<?xml version=\"1.0\"?>\n");
	EXPECT_BUF(buf_compact, "<?xml version=\"1.0\"?>\n");
	EXPECT_BUF(buf_tlist, "");

	XPARSE(1, "<?name data?>");
	EXPECT_BUF(buf_dump,
		"pi     0 0 path=\"\" name=\"name\" data=\"data\"\n"
		"*BREAK*\n"
	);
	EXPECT_BUF(buf_print, "<?name data?>\n");
	EXPECT_BUF(buf_compact, "<?name data?>\n");
	EXPECT_BUF(buf_tlist, "");

	PARSE("<root><?name data?></root>");
	EXPECT_BUF(buf_dump,
		"beg    0 0 path=\"/root\" name=\"root\" data=\"\"\n"
		"pi     1 1 path=\"/root\" name=\"name\" data=\"data\"\n"
		"end    0 0 path=\"/root\" name=\"root\" data=\"\"\n"
	);
	EXPECT_BUF(buf_print,
		"<root>\n"
		"  <?name data?>\n"
		"</root>\n"
	);
	EXPECT_BUF(buf_compact,
		"<root><?name data?>\n"
		"</root>"
	);
	EXPECT_BUF(buf_tlist, "+root\n/root\n");

	XPARSE(2, "<root><?name data?></root>");
	EXPECT_BUF(buf_dump,
		"beg    0 0 path=\"/root\" name=\"root\" data=\"\"\n"
		"pi     1 1 path=\"/root\" name=\"name\" data=\"data\"\n"
		"*BREAK*\n"
	);
	EXPECT_BUF(buf_print,
		"<root>\n"
		"  <?name data?>\n"
	);
	EXPECT_BUF(buf_compact,
		"<root><?name data?>\n"
	);
	EXPECT_BUF(buf_tlist, "+root\n");
}

TEST_DECL(special)
{
	PARSE("<!");
	EXPECT_BUF(buf_dump,
		"err    0 0 path=\"\" name=\"\" "
			"data=\"unexpected end of file\"\n"
	);
	EXPECT_BUF(buf_print,
		"<!--\n"
		"ERROR: unexpected end of file\n"
		"-->\n"
	);
	EXPECT_BUF(buf_compact, "");
	EXPECT_BUF(buf_tlist, "");

	PARSE("<!- x");
	EXPECT_BUF(buf_dump,
		"err    0 0 path=\"\" name=\"\" "
			"data=\"EOF in declaration: '-x'\"\n"
	);
	EXPECT_BUF(buf_print,
		"<!--\n"
		"ERROR: EOF in declaration: '-x'\n"
		"-->\n"
	);
	EXPECT_BUF(buf_compact, "");
	EXPECT_BUF(buf_tlist, "");

	PARSE("<!- x>");
	EXPECT_BUF(buf_dump,
		"err    0 0 path=\"\" name=\"\" "
			"data=\"undefined declaration: '-x'\"\n"
	);
	EXPECT_BUF(buf_print,
		"<!--\n"
		"ERROR: undefined declaration: '-x'\n"
		"-->\n"
	);
	EXPECT_BUF(buf_compact, "");
	EXPECT_BUF(buf_tlist, "");

	PARSE("<!--x>");
	EXPECT_BUF(buf_dump,
		"err    0 0 path=\"\" name=\"\" "
			"data=\"EOF in comment: 'x>'\"\n"
	);
	EXPECT_BUF(buf_print,
		"<!--\n"
		"ERROR: EOF in comment: 'x>'\n"
		"-->\n"
	);
	EXPECT_BUF(buf_compact, "");
	EXPECT_BUF(buf_tlist, "");

	PARSE("<!-- com&apos;ment -->");
	EXPECT_BUF(buf_dump,
		"comm   0 0 path=\"\" name=\"\" data=\" com'ment \"\n"
	);
	EXPECT_BUF(buf_print, "<!-- com'ment -->\n");
	EXPECT_BUF(buf_compact, "");
	EXPECT_BUF(buf_tlist, "");

	XPARSE(1, "<!-- com&apos;ment -->");
	EXPECT_BUF(buf_dump,
		"comm   0 0 path=\"\" name=\"\" data=\" com'ment \"\n"
		"*BREAK*\n"
	);
	EXPECT_BUF(buf_print, "<!-- com'ment -->\n");
	EXPECT_BUF(buf_compact, "");
	EXPECT_BUF(buf_tlist, "");

	PARSE("<root><!-- comment --></root>");
	EXPECT_BUF(buf_dump,
		"beg    0 0 path=\"/root\" name=\"root\" data=\"\"\n"
		"comm   1 1 path=\"/root\" name=\"root\" data=\" comment \"\n"
		"end    0 0 path=\"/root\" name=\"root\" data=\"\"\n"
	);
	EXPECT_BUF(buf_print,
		"<root>\n"
		"  <!-- comment -->\n"
		"</root>\n"
	);
	EXPECT_BUF(buf_compact, "<root></root>");
	EXPECT_BUF(buf_tlist, "+root\n/root\n");

	XPARSE(2, "<root><!-- comment --></root>");
	EXPECT_BUF(buf_dump,
		"beg    0 0 path=\"/root\" name=\"root\" data=\"\"\n"
		"comm   1 1 path=\"/root\" name=\"root\" data=\" comment \"\n"
		"*BREAK*\n"
	);
	EXPECT_BUF(buf_print,
		"<root>\n"
		"  <!-- comment -->\n"
	);
	EXPECT_BUF(buf_compact, "<root>");
	EXPECT_BUF(buf_tlist, "+root\n");

	PARSE("<!abc&lt;&undef;&gt;>");
	EXPECT_BUF(buf_dump, 
		"err    0 0 path=\"\" name=\"\" "
			"data=\"undefined declaration: 'abc<&undef;>'\"\n"
	);
	EXPECT_BUF(buf_print,
		"<!--\n"
		"ERROR: undefined declaration: 'abc<&undef;>'\n"
		"-->\n"
	);
	EXPECT_BUF(buf_compact, "");
	EXPECT_BUF(buf_tlist, "");

	PARSE("<!DOCTYPE xml SYSTEM");
	EXPECT_BUF(buf_dump,
		"err    0 0 path=\"\" name=\"\" "
			"data=\"EOF in DOCTYPE: 'xml SYSTEM'\"\n"
	);
	EXPECT_BUF(buf_print,
		"<!--\n"
		"ERROR: EOF in DOCTYPE: 'xml SYSTEM'\n"
		"-->\n"
	);
	EXPECT_BUF(buf_compact, "");
	EXPECT_BUF(buf_tlist, "");


	PARSE("<!DOCTYPE xml SYSTEM \"/dev/null\" >");
	EXPECT_BUF(buf_dump,
		"dtd    0 0 path=\"\" "
			"name=\"xml SYSTEM \\\"/dev/null\\\" \" data=\"\"\n"
	);
	EXPECT_BUF(buf_print,
		"<!DOCTYPE xml SYSTEM \"/dev/null\" >\n");
	EXPECT_BUF(buf_compact, "<!DOCTYPE xml SYSTEM \"/dev/null\" >\n");
	EXPECT_BUF(buf_tlist, "");

	PARSE("<![CDATA[ any");
	EXPECT_BUF(buf_dump,
		"err    0 0 path=\"\" name=\"\" "
			"data=\"EOF in CDATA: ' any'\"\n"
	);
	EXPECT_BUF(buf_print,
		"<!--\n"
		"ERROR: EOF in CDATA: ' any'\n"
		"-->\n"
	);
	EXPECT_BUF(buf_compact, "");
	EXPECT_BUF(buf_tlist, "");

	PARSE("<![CDATA[ any <data> &lt; ]]>");
	EXPECT_BUF(buf_dump,
		"cdata  0 0 path=\"\" name=\"\" data=\" any <data> &lt; \"\n"
	);
	EXPECT_BUF(buf_print, "<![CDATA[ any <data> &lt; ]]>\n");
	EXPECT_BUF(buf_compact, "<![CDATA[ any <data> &lt; ]]>");
	EXPECT_BUF(buf_tlist, "\t any <data> &lt; \n");
}

TEST_DECL(att)
{
	PARSE("<x");
	EXPECT_BUF(buf_dump,
		"err    0 0 path=\"\" name=\"\" "
			"data=\"EOF in tag: 'x'\"\n"
	);
	EXPECT_BUF(buf_print,
		"<!--\n"
		"ERROR: EOF in tag: 'x'\n"
		"-->\n"
	);
	EXPECT_BUF(buf_compact, "");
	EXPECT_BUF(buf_tlist, "");

	PARSE("<x ");
	EXPECT_BUF(buf_dump,
		"beg    0 0 path=\"/x\" name=\"x\" data=\"\"\n"
		"err    1 1 path=\"/x\" name=\"x\" "
			"data=\"unexpected end of file\"\n"
	);
	EXPECT_BUF(buf_print,
		"<x>\n"
		"  <!--\n"
		"ERROR: unexpected end of file\n"
		"  -->\n"
	);
	EXPECT_BUF(buf_compact, "<x>");
	EXPECT_BUF(buf_tlist, "+x\n");

	PARSE("<x=>");
	EXPECT_BUF(buf_dump,
		"beg    0 0 path=\"/x\" name=\"x\" data=\"\"\n"
		"err    1 1 path=\"/x\" name=\"x\" "
			"data=\"unexpected character '='\"\n"
	);
	EXPECT_BUF(buf_print,
		"<x>\n"
		"  <!--\n"
		"ERROR: unexpected character '='\n"
		"  -->\n"
	);
	EXPECT_BUF(buf_compact, "<x>");
	EXPECT_BUF(buf_tlist, "+x\n");

	PARSE("<x/>\n");
	EXPECT_BUF(buf_dump,
		"beg    0 0 path=\"/x\" name=\"x\" data=\"\"\n"
		"end    0 1 path=\"/x\" name=\"x\" data=\"\"\n"
	);
	EXPECT_BUF(buf_print,
		"<x/>\n"
	);
	EXPECT_BUF(buf_compact, "<x/>");
	EXPECT_BUF(buf_tlist, "+x\n/x\n");

	XPARSE(1, "<x/>\n");
	EXPECT_BUF(buf_dump,
		"beg    0 0 path=\"/x\" name=\"x\" data=\"\"\n"
		"*BREAK*\n"
	);
	EXPECT_BUF(buf_print, "<x");
	EXPECT_BUF(buf_compact, "<x");
	EXPECT_BUF(buf_tlist, "+x\n");

	PARSE("<x/y>\n");
	EXPECT_BUF(buf_dump,
		"beg    0 0 path=\"/x\" name=\"x\" data=\"\"\n"
		"err    1 1 path=\"/x\" name=\"x\" "
			"data=\"unexpected character 'y'\"\n"
	);
	EXPECT_BUF(buf_print,
		"<x>\n"
		"  <!--\n"
		"ERROR: unexpected character 'y'\n"
		"  -->\n"
	);
	EXPECT_BUF(buf_compact, "<x>");
	EXPECT_BUF(buf_tlist, "+x\n");

	PARSE("<x a='&#64;data' b c! d=: e='y ");
	EXPECT_BUF(buf_dump,
		"beg    0 0 path=\"/x\" name=\"x\" data=\"\"\n"
		"att    1 1 path=\"/x\" name=\"a\" data=\"@data\"\n"
		"err    1 1 path=\"/x\" name=\"x\" "
			"data=\"unexpected character 0x20\"\n"
		"err    1 0 path=\"/x\" name=\"x\" "
			"data=\"unexpected character '!'\"\n"
		"err    1 0 path=\"/x\" name=\"x\" "
			"data=\"unexpected character ':'\"\n"
		"err    1 0 path=\"/x\" name=\"e\" "
			"data=\"unexpected end of file\"\n"
		"err    1 0 path=\"/x\" name=\"e\" "
			"data=\"unexpected end of file\"\n"
	);
	EXPECT_BUF(buf_print,
		"<x a=\"@data\">\n"
		"  <!--\n"
		"ERROR: unexpected character 0x20\n"
		"  -->\n"
		"  <!--\n"
		"ERROR: unexpected character '!'\n"
		"  -->\n"
		"  <!--\n"
		"ERROR: unexpected character ':'\n"
		"  -->\n"
		"  <!--\n"
		"ERROR: unexpected end of file\n"
		"  -->\n"
		"  <!--\n"
		"ERROR: unexpected end of file\n"
		"  -->\n"
	);
	EXPECT_BUF(buf_compact, "<x a=\"@data\">");
	EXPECT_BUF(buf_tlist, "+x\n");

	XPARSE(2, "<x a='&#64;data' b c! d=: e='y ");
	EXPECT_BUF(buf_dump,
		"beg    0 0 path=\"/x\" name=\"x\" data=\"\"\n"
		"att    1 1 path=\"/x\" name=\"a\" data=\"@data\"\n"
		"*BREAK*\n"
	);
	EXPECT_BUF(buf_print, "<x a=\"@data\"");
	EXPECT_BUF(buf_compact, "<x a=\"@data\"");
	EXPECT_BUF(buf_tlist, "+x\n");
}

TEST_SUITE
{
//	TEST_RUN(create);
	TEST_RUN(instruction);
	TEST_RUN(special);
	TEST_RUN(doc);
	TEST_RUN(main);
	TEST_RUN(att);
}
