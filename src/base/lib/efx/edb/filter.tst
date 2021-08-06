#!/usr/bin/env .build/script.tst

str dbfilter(str fdef, str in)
{
	StrBuf buf;
	EDB edb = EDB((IO) in).filter(fdef);
	edb.out(buf, "plain");
	edb.copy(0);
	return buf;
}

str syntax(str fdef)
{
	UnitTest::stdout("stdout");
	Info("print:EDB/filter/" + fdef);
	UnitTest::stdout(NULL);
	return UnitTest::content("stdout");
}

TEST(clip)
{
	MATCH_STR(syntax("clip"), "clip=skip,count");
	EXPECT_STR(dbfilter("clip=1,2", string !
EDB	1.0
int
@data plain
12
17
23
42
!), string !
EDB	1.0
int
@data plain
17
23
!);
	EXPECT_STR(dbfilter("clip=2", string !
EDB	1.0
int
@data plain
12
17
23
42
!), string !
EDB	1.0
int
@data plain
23
42
!);
}

TEST(dup)
{
	MATCH_STR(syntax("dup"), "dup=list");
	EXPECT_STR(dbfilter("dup=b,c=99,d=X", string !
EDB	1.0
int a;
int b;
int c;
enum { A = 1, B = 2 } d;
@data plain
! + "#a;b;c;d\n" + string !
11;12;13;"A"
21;22;23;"B"
!), string !
EDB	1.0
int a;
int b;
int c;
enum {
	A = 1,
	B = 2,
	X = 3,
} d;
@data plain
! + "#a;b;c;d\n" + string !
11;12;13;1
11;0;13;1
11;12;99;1
11;12;13;3
21;22;23;2
21;0;23;2
21;22;99;2
21;22;23;3
!);
}

TEST(expr)
{
	MATCH_STR(syntax("expr"), "expr\\[commands]");
	EXPECT_STR(dbfilter(string !
expr[
	if (a < 11) continue;
	if (a > 30) break;
	c += 5;
]
!, string !
EDB	1.0
int a;
int b;
int c;
@data plain
! + "#a;b;c\n" + string !
1;2;3
11;12;13
21;22;23
31;32;33
41;42;43
!), string !
EDB	1.0
int a;
int b;
int c;
@data plain
! + "#a;b;c\n" + string !
11;12;18
21;22;28
!);
}

TEST(nest)
{
	MATCH_STR(syntax("nest"), "nest\\[name=list]");
	EXPECT_STR(dbfilter("nest[x=a,c d[uniq]]", string !
EDB	1.0
int a;
int b;
int c;
int d;
@data plain
! + "#a;b;c;d\n" + string !
11;12;13;14
11;12;13;14
21;22;23;24
31;22;33;34
!), string !
EDB	1.0
int b;
struct {
int a;
int c;
}[] x;
int[] d;
@data plain
! + "#b;x;d\n" + string !
12;{{11, 13}, {11, 13}};{14}
22;{{21, 23}, {31, 33}};{24, 34}
!);
}

TEST(sample)
{
	MATCH_STR(syntax("sample"), "sample\\[rng;cmp]=weight");
	EXPECT_STR(dbfilter("sample[:std:0X00002A;cmp:b]=50", string !
EDB	1.0
int a;
int b;
@data plain
! + "#a;b\n" + string !
1;1
2;1
3;1
4;2
5;2
6;3
7;3
8;3
9;4
10;5
!), string !
EDB	1.0
int a;
int b;
@data plain
! + "#a;b\n" + string !
6;3
7;3
8;3
10;5
!);
}

TEST(sum)
{
	MATCH_STR(syntax("sum"), "sum=list");
	EXPECT_STR(dbfilter("sum=a", string !
EDB	1.0
int a;
int b;
@data plain
! + "#a;b\n" + string !
1;1
2;1
3;1
4;2
5;2
6;3
7;3
8;3
9;4
10;5
!), string !
EDB	1.0
int a;
int b;
@data plain
! + "#a;b\n" + string !
6;1
9;2
21;3
9;4
10;5
!);
}

TEST(tee)
{
	MATCH_STR(syntax("tee"), "tee\\[mode]=path");
	EXPECT_STR(dbfilter(UnitTest::tpl("tee[nohead]=@/data"), string !
EDB	1.0
int
@data plain
12
17
23
42
!), string !
EDB	1.0
int
@data plain
12
17
23
42
!);
	EXPECT_CONTENT("data", string !
12
17
23
42
!);
}

TEST(test)
{
	MATCH_STR(syntax("test"), "test\\[expr]");
	EXPECT_STR(dbfilter("test[this%2==0]", string !
EDB	1.0
int
@data plain
12
17
23
42
!), string !
EDB	1.0
int
@data plain
12
42
!);
}

TEST(uniq)
{
	MATCH_STR(syntax("uniq"), "uniq");
	EXPECT_STR(dbfilter("uniq", string !
EDB	1.0
int
@data plain
12
17
17
17
23
17
42
!), string !
EDB	1.0
int
@data plain
12
17
23
17
42
!);
}

TEST(var)
{
	MATCH_STR(syntax("var"), "var\\[r]=list");
	EXPECT_STR(dbfilter("var=a,x.b,*,z=5", string !
EDB	1.0
int a;
struct {
	int b;
	int c;
} x;
int d;
@data plain
! + "#a;x;d\n" + string !
11;{12, 13};14
21;{22, 23};24
!), string !
EDB	1.0
int a;
int b;
struct {
	int b;
	int c;
} x;
int d;
int z;
@data plain
! + "#a;b;x;d;z\n" + string !
11;12;{12, 13};14;5
21;22;{22, 23};24;5
!);
	EXPECT_STR(dbfilter("var=x.c", string !
EDB	1.0
int a;
struct {
	int b;
	int c;
} x;
int d;
@data plain
! + "#a;x;d\n" + string !
11;{12, 13};14
21;{22, 23};24
!), string !
EDB	1.0
int c;
@data plain
! + "#c\n" + string !
13
23
!);
	EXPECT_STR(dbfilter("var[r]=x.c", string !
EDB	1.0
int a;
struct {
	int b;
	int c;
} x;
int d;
@data plain
! + "#a;x;d\n" + string !
11;{12, 13};14
21;{22, 23};24
!), string !
EDB	1.0
int
@data plain
13
23
!);
}
