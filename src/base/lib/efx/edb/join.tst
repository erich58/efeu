#!/usr/bin/env .build/script.tst

str dbjoin(str jdef)
{
	StrBuf buf;
	EDB edb = edbjoin(edbopen(UnitTest::tmp("a.db")),
		edbopen(UnitTest::tmp("b.db")), jdef);
	edb.out(buf, "plain");
	edb.copy(0);
	edb.closein();
	edb.closeout();
	return buf;
}

TEST(bad)
{
	TEST_FILE("a.db", string !
EDB	1.0
int a;
@data plain
! + "#a\n" + string !
1
3
2
5
!);
	TEST_FILE("b.db", "EDB\t1.0\nint a;\n");

	TEST_FORK
	{
		EXPECT_STR(dbjoin(NULL), NULL);
	}
	CHILD_EXIT(1);

	TEST_FORK
	{
		EXPECT_STR(dbjoin("key=x"), NULL);
	}
	CHILD_EXIT(1);
}

TEST(simple)
{
	TEST_FILE("a.db", string !
EDB	1.0
int k;
int a;
@data plain
! + "#k;a\n" + string !
1;101
2;102
3;103
5;105
!);
	TEST_FILE("b.db", string !
EDB	1.0
int k;
int b;
@data plain
! + "#k;b\n" + string !
1;201
2;202
4;204
5;205
!);

	EXPECT_STR(dbjoin(NULL), string !
EDB	1.0
int k;
int a;
int b;
@data plain
! + "#k;a;b\n" + string !
1;101;201
2;102;202
5;105;205
!);
	EXPECT_STR(dbjoin("mode=a"), string !
EDB	1.0
int k;
int a;
@data plain
! + "#k;a\n" + string !
3;103
!);
	EXPECT_STR(dbjoin("mode=bx"), string !
EDB	1.0
int k;
int a;
int b;
@data plain
! + "#k;a;b\n" + string !
1;101;201
2;102;202
4;0;204
5;105;205
!);
}
