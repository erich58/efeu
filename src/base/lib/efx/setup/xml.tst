#!/usr/bin/env .build/script.tst

XML x;
fprint(OUT(), x);
EXPECT_OUT("XMLBuf(0x0, 0x0, 0, 0)");
print(OUT(), x);
EXPECT_OUT("");

UnitTest::stderr("stderr");
x.open(ionull, "?");
UnitTest::stderr();
MATCH_CONTENT("stderr", "c:ompact.*b:eautified.*t:list.*d:ump");

x.open(OUT(), "b");
x.dtd("html public"); 
x.pi("extra", " "); 
x.close();
EXPECT_OUT("<!DOCTYPE html public>\n<?extra?>\n");

x.open(OUT(), "ubk");
x.rem("fmt %s", "arg"); 
x.err("fmt %s", "arg"); 
x.close();
EXPECT_OUT("<!--fmt arg-->\n<!--\nERROR: fmt arg\n-->\n");

x.open(OUT(), "c");
x.beg("root"); 
x.att("att", "value");
x.entry("tag", "data");
x.end();
x.close();
EXPECT_OUT("<root att=\"value\"><tag>data</tag></root>");

x.open(OUT(), "c");
x.beg("root"); 
x.data("data");
x.cdata("data");
x.close();
EXPECT_OUT("<root>data<![CDATA[data]]></root>");

x.open(OUT(), "c");
x.parse("<root><tag>data</tag></root>");
x.close();
EXPECT_OUT("<root><tag>data</tag></root>");

UnitTest::stdout("stdout");
x.expr([ {printf("%s: %s=%s\n", type, name, data);} ]);
x.parse("<root><tag>data</tag></root>");
x.close();
UnitTest::stdout();
EXPECT_CONTENT("stdout",
	"beg: root=\nbeg: tag=\ndata: tag=data\nend: tag=\nend: root=\n");

free(x);
