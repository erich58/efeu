#!/usr/bin/env .build/script.tst

TEST_FILE("data", string !
1234567890
abcdefghij
!);

MapFile map = NULL;
EXPECT_REPR(map, "NULL");
EXPECT(map.size == 0);
EXPECT(map.name == NULL);

EXPECT(map = UnitTest::tmp("data"));
EXPECT_STR(map.name, UnitTest::tmp("data"));
EXPECT(map.size == 22);
EXPECT(IO io = map);
EXPECT_STR(io.gets(), "1234567890");
EXPECT_STR(io.gets(), "abcdefghij");
EXPECT_STR(io.gets(), NULL);
EXPECT(IO io = IO(map, 4, 2));
EXPECT_STR(io.gets(), "56");
EXPECT_STR(io.gets(), NULL);
EXPECT(IO io = IO(map, 15, 0));
EXPECT_STR(io.gets(EOF), "efghij\n");
EXPECT(IO(map, 22) == NULL);
EXPECT(IO(map, 0, 23) == NULL);

UnitTest::stderr("stderr");
EXPECT(MapFile(UnitTest::tmp("bad/file/name")) == NULL);
UnitTest::stderr();
MATCH_CONTENT("stderr", "MapFile\(.*/bad/file/name\"\):");
