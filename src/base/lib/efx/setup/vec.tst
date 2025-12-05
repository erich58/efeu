#!/usr/bin/env .build/script.tst

enum INDEX { first, second };
int[] x = 10 : 17 : 2;
EXPECT(x.dim == 4);
EXPECT(dim(x) == 4);
EXPECT(x.type == int);
EXPECT_STR(x, "{10, 12, 14, 16}");
EXPECT(x[0] == 10);
EXPECT(x[INDEX::second] == 12);
EXPECT_STR(x[1:2], "{12, 14}");
x := 100 : 200 : 100;
EXPECT_STR(x, "{100, 200, 14, 16}");
x += { 0, 0, 300, 400 };
EXPECT_STR(x, "{100, 200, 314, 416}");
x.resize(3);
EXPECT_STR(x, "{100, 200, 314}");
x.delete(0, 2);
EXPECT_STR(x, "{314}");
x.append(20);
EXPECT_STR(x, "{314, 20}");
CmpDef c = CmpDef(int, "+");
x.qsort(c)
EXPECT_STR(x, "{20, 314}");
qsort(x, "-");
EXPECT_STR(x, "{314, 20}");
x = { 1, 2, 2, 2, 3, 4 };
uniq(x, "-", true);
EXPECT_STR(x, "{4, 3, 2, 1}");
x.append(3);
x.append(3);
EXPECT_STR(x, "{4, 3, 2, 1, 3, 3}");
x.uniq(c, true);
EXPECT_STR(x, "{1, 2, 3, 4}");

struct S { int a; str r };
S[] s = { { 1, "I" }, { 2, "II" }, { 3, "III" }, { 4, "IV" }, { 5, "V" }};
polymorph int scmp(int k, S s) return cmp(k, s.a);
EXPECT_STR(bsearch(s, 2, scmp), "{2, \"II\"}");
EXPECT_STR(s.bsearch(3, scmp), "{3, \"III\"}");
EXPECT_REPR(s.bsearch(10, scmp), "NULL");
s.resize(0);
EXPECT_REPR(s.bsearch(0, scmp), "NULL");
