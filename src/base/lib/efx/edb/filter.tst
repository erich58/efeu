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
