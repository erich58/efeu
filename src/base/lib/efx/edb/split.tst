#!/usr/bin/env .build/script.tst

TEST(split)
{
	TEST_DIR("dir");
	int x = 0;
	EDB edb = EDB(x, "DESC\nLINE 2\n");
	edb.fout(UnitTest::tmp("dir/db.edb"), "plain,split=5");
	str hdr = "EDB\t1.0\n// DESC\n// LINE 2\n";

	for (x = 0; x < 5; ++x)
		edb.write();

	edb = NULL;
	EXPECT_CONTENT("dir/db.edb", hdr + string !
@file
db.01.edb
db.02.edb
!);
	EXPECT_CONTENT("dir/db.01.edb", hdr + string !
int
@data plain
0
1
2
!);
	EXPECT_CONTENT("dir/db.02.edb", hdr + string !
int
@data plain
3
4
!);
}
