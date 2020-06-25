#!/usr/bin/env .build/script.tst

CmpDef c;
LogConfig(UnitTest::tpl("CmpDef.note=@/log"));

c = CmpDef(int, ".");
EXPECT_REPR(c, "CmpDef: int +0:1*4 int {cmp()}");

EXPECT(c(1, 5) == -1);
EXPECT(c(5, 1) == 1);
EXPECT(c(2, 2) == 0);

EXPECT(c(5, 2, 7) == -1);
EXPECT(c(5, 5, 7) == 0);
EXPECT(c(5, 7, 7) == 0);
EXPECT(c(5, 8, 7) == 1);

EXPECT(c() == 0);
EXPECT(c(5) == 0);

c = CmpDef(varint, "-");
EXPECT_REPR(c, "CmpDef: varint -0:1*8 varint {cmp()}");
EXPECT(c(1, 5) == 1);
EXPECT(c(5, 1) == -1);
EXPECT(c(2, 2) == 0);

struct X {
	int a;
	int[2] v;
	str b;
};

X x1 = { 1, { 0, 1 },"A" };
X x2 = { 2, { 1, 2 }, "B" };
X x3 = { 2, { 2, 1 }, "C" };

EXPECT(cmp(x1, x2) == -1);

int f1(X a, X b)
	return cmp(a.a, b.a);

c = CmpDef(X, "+f1:");
EXPECT(c(x1, x2) == -1);
EXPECT(c(x2, x3) == 0);

c = CmpDef(X, "(a),b");
PRINT(c);
EXPECT(c(x1, x2) == -1);
EXPECT(c(x2, x3) == -1);

c = CmpDef(X, "-b");
PRINT(c);
EXPECT(c(x1, x2) == 1);
EXPECT(c(x2, x3) == 1);

c = CmpDef(X, "v[1]");
PRINT(c);
EXPECT(c(x1, x2) == -1);
EXPECT(c(x2, x3) == 1);
EXPECT(c(x1, x3) == 0);

EXPECT_REPR(CmpDef(X, ""), "CmpDef: X");
EXPECT_REPR(CmpDef(X, "."), "CmpDef: X +0:1*24 X {cmp()}");
EXPECT_REPR(CmpDef(X, "rcmp:v"), "CmpDef: X +8:2*4 int {rcmp()}");
EXPECT_REPR(CmpDef(int[2], "cmp:"), "CmpDef: _int2 +0:2*4 int {int +0:1*4 int {cmp()}}");

#if	0
TEST_FILE("log", NULL);
EXPECT_REPR(c = CmpDef(int, "f1:"), "CmpDef: int +0:1*4 int {0x0}");
EXPECT_CONTENT("log", "CmpDef: function f1() not defined for type \"int\".\n");
#endif

TEST_FILE("log", NULL);
EXPECT_REPR(c = CmpDef(X, "v[2]"),
	"CmpDef: X +8:2*4 int {cmp()}");
EXPECT_CONTENT("log",
	"CmpDef: vector index 2 out of range.\n");

// Binary cmp
TEST_FILE("log", NULL);
EXPECT_REPR(c = CmpDef(FileStat, "."),
	"CmpDef: FileStat +0:1*40 FileStat {0x0}");
FileStat fs1 = FileStat(".");
FileStat fs2 = FileStat(".");
EXPECT(c(fs1, fs2) != 0);
EXPECT_CONTENT("log",
	"CmpDef: function cmp() not defined for type \"FileStat\".\n");

CmpDef(int[2][2], ".,(.[0],.[1][0])");
CmpDef(struct { int x[2][2]; }, "x[1]");
