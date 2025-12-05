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

#define	EXPECT_BUF(a, b) \
	UnitTest_strcheck(__FILE__, __LINE__, "==", buf_data(&a), b, # a, # b)

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

static void *test_action(XMLBuf *xml, const char *name,
	const char *data, void *par)
{
	xml_print(xml, name, data, &out_print);
	xml_compact(xml, name, data, &out_compact);
	xml_tlist(xml, name, data, &out_tlist);
	xml_dump(xml, name, data, &out_dump);
	return NULL;
}

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

TEST_DECL(parse)
{
	XMLBuf *x;
	EXPECT_PTR(x = xml_create(NULL, test_action, NULL));
	XMLBuf_parse(x, NULL);
	EXPECT_BUF(buf_print, "");
	EXPECT_BUF(buf_compact, "");
	EXPECT_BUF(buf_tlist, "");
	EXPECT_BUF(buf_dump, "");

	XMLBuf_parse(x, IN(
		"<!doctype html>\n"
		"<?xml version=\"1.0\"?>\n"
		));
	EXPECT_BUF(buf_print,
		"<!DOCTYPE html>\n"
		"<?xml version=\"1.0\"?>\n"
	);
	EXPECT_BUF(buf_compact,
		"<!DOCTYPE html>\n"
		"<?xml version=\"1.0\"?>\n"
	);
	EXPECT_BUF(buf_tlist, "");
	EXPECT_BUF(buf_dump,
		"dtd    0 0 path=\"\" name=\"html\" data=\"\"\n"
		"pi     0 0 path=\"\" name=\"xml\" data=\"version=\\\"1.0\\\"\"\n"
	);

	XMLBuf_parse(x, IN(
		"<root>"
		"&amp;&euro;&#xC4;&#64;data"
		"</root>\n"
		));
	EXPECT_BUF(buf_print,
		"<root>\n"
		"  &amp;\342\202\254\303\204@data\n"
		"</root>\n"
	);
	EXPECT_BUF(buf_compact,
		"<root>"
		"&amp;\342\202\254\303\204@data"
		"</root>"
	);
	EXPECT_BUF(buf_tlist,
		"+root\n"
		"\t&\342\202\254\303\204@data\n"
		"/root\n"
	);
	EXPECT_BUF(buf_dump,
		"beg    0 0 path=\"/root\" name=\"root\" data=\"\"\n"
		"data   1 1 path=\"/root\" name=\"root\" data=\"&\342\202\254\303\204@data\"\n"
		"end    0 0 path=\"/root\" name=\"root\" data=\"\"\n"
	);
}

TEST_SUITE
{
	TEST_RUN(create);
	TEST_RUN(parse);
}
