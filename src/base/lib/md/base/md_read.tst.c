#include <EFEU/UnitTest.h>
#include <EFEU/mdmat.h>
#include "utest/md.tst.h"
#include "utest/in.tst.h"

#define	TEST_DATA(var, def)	\
var = def, PRINT_STR(var)

TEST_DECL(null)
{
	EXPECT(md_read(IN(""), NULL) == NULL);
	EXPECT(md_read(IN("\n"), NULL) == NULL);
}

TEST_DECL(binary)
{
	mdmat *md = mdtest_create("name x=a,b,c");
	IO *io = io_tmpbuf(0);
	md_save(io, md, 0);
	rd_deref(md);

	io_rewind(io);
	md = md_read(io, NULL);
	io_close(io);
	EXPECT_STR(mdtest_show(md, "hd"),
		"int[3] \"name\"\n"
		"x           # int[3]\n"
		"a           0 1\n"
		"b           1 2\n"
		"c           2 3\n"
	);
	rd_deref(md);
}


TEST_DECL(par)
{
	mdmat *md; 

	EXPECT_PTR(md = md_read(IN("z a\nb 42\n"), NULL));
	MATCH_STR(mdtest_show(md, "d"),
		"y *x *# double\\[1]\n"
		"b *a *0 42.0*\n"
	);
	rd_deref(md);

	EXPECT(md_read(IN("z a\nb 42\n"), "magic") == NULL);

	EXPECT_PTR(md = md_read(IN("z a\nb 42\n"), "nomagic"));
	MATCH_STR(mdtest_show(md, "d"),
		"y *x *# double\\[1]\n"
		"b *a *0 42.0*\n"
	);
	rd_deref(md);

	EXPECT_PTR(md = md_read(IN("##MDMAT 3.0\nz a\nb 42\n"), "magic"));
	MATCH_STR(mdtest_show(md, "d"),
		"y *x *# double\\[1]\n"
		"b *a *0 42.0*\n"
	);
	rd_deref(md);

	EXPECT_PTR(md = md_read(IN("z a\nb 2,5\n"), "locale=de"));
	MATCH_STR(mdtest_show(md, "d"),
		"y *x *# double\\[1]\n"
		"b *a *0 2.50*\n"
	);
	rd_deref(md);

	EXPECT_PTR(md = md_read(IN("z a\nb 2.5\n"), "locale=C"));
	MATCH_STR(mdtest_show(md, "d"),
		"y *x *# double\\[1]\n"
		"b *a *0 2.50*\n"
	);
	rd_deref(md);

	EXPECT_PTR(md = md_read(IN("z a\nb 42\n"),
		"type=int x=X-Axis y=Y-Axis"));
	MATCH_STR(mdtest_show(md, "d"),
		"Y-Axis *X-Axis *# int\\[1]\n"
		"b *a *0 42\n"
	);
	rd_deref(md);
}

TEST_DECL(head)
{
	mdmat *md; 

	TEST_FILE("data", 
		"##MDMAT 3.0\n"
		"\n"
		"##Unknown directive\n"
		"##Title Matrix Title\n"
		"##Head /**/\n"
		"##Desc Description\n"
		"##Type int\n"
		"##Locale C\n"
		"##Lines z y\n"
		"##Colums x\n"
		"2 x1 x2\n"
		"z1.y1 111\n"
		"# Comment\n"
		": Comment\n"
		"; Comment\n"
		"\n"
		"z2.y2 221 222\n"
	);
	EXPECT_PTR(md = md_fread(UnitTest_tmp("data"), NULL));
	EXPECT_STR(mdtest_show(md, "hd"),
		"int[8=2*2*2] \"Matrix Title\" /* Description */\n"
		"z       y       x           # int[8]\n"
		"z1      y1      x1          0 111\n"
		"z1      y1      x2          1 0\n"
		"z1      y2      x1          2 0\n"
		"z1      y2      x2          3 0\n"
		"z2      y1      x1          4 0\n"
		"z2      y1      x2          5 0\n"
		"z2      y2      x1          6 221\n"
		"z2      y2      x2          7 222\n"
	);
	rd_deref(md);
}

TEST_DECL(structure)
{
	mdmat *md; 
	TEST_FILE("data", 
		"##Type struct { int a; int b; }\n"
		"2 x1.a x1.b x2.a x2.b\n"
		"y1 1 2 3 4\n"
		"y2 11 :  : 13 ; 14\n"
	);
	EXPECT_PTR(md = md_fread(UnitTest_tmp("data"), NULL));
	EXPECT_STR(mdtest_show(md, "d"),
		"y       x           # struct { int a, b }[4]\n"
		"y1      x1          0 {1, 2}\n"
		"y1      x2          1 {3, 4}\n"
		"y2      x1          2 {11, 0}\n"
		"y2      x2          3 {13, 14}\n"
	);
	rd_deref(md);
}

TEST_DECL(term)
{
	mdmat *md; 
	TEST_FILE("data", 
		"##Type int\n"
		"2 x[1] x[2]\n"
		"y1 1 2\n"
		"y2 3 (5 * (2 + 1))\n"
	);
	EXPECT_PTR(md = md_fread(UnitTest_tmp("data"), NULL));
	EXPECT_STR(mdtest_show(md, "d"),
		"y       x           # int[4]\n"
		"y1      x[1]        0 1\n"
		"y1      x[2]        1 2\n"
		"y2      x[1]        2 3\n"
		"y2      x[2]        3 15\n"
	);
	rd_deref(md);
}

TEST_DECL(label)
{
	mdmat *md; 
	TEST_FILE("data", 
		"##Type int\n"
		"2 x{[1]} x[(2)] x({3}) x\\a\\:\n"
		"y 1 2 3 4\n"
	);
	EXPECT_PTR(md = md_fread(UnitTest_tmp("data"), NULL));
	EXPECT_STR(mdtest_show(md, "d"),
		"y       x           # int[4]\n"
		"y       x{[1]}      0 1\n"
		"y       x[(2)]      1 2\n"
		"y       x({3})      2 3\n"
		"y       x\\a:        3 4\n"
	);
	rd_deref(md);
}

TEST_SUITE
{
	setenv("LANG", "C", 1);
	SetupMdMat();
	SetProgName("ID");

	TEST_RUN(null);
	TEST_RUN(binary);
	TEST_RUN(par);
	TEST_RUN(head);
	TEST_RUN(structure);
	TEST_RUN(term);
	TEST_RUN(label);

	CLEANUP_IN;
}
