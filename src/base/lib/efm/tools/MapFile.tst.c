#include <EFEU/UnitTest.h>
#include <EFEU/MapFile.h>
#include <EFEU/procenv.h>

#define	TEST_DATA	"1234567890ABCDEFGHIJKLMNOPQRSTUVMXYZ\n"

TEST_SUITE
{
	SetProgName(UnitTest_name);

	MapFile *map;
	EXPECT_TRUE(MapFile_open("/unknown file") == NULL);
	EXPECT_TRUE(MapFile_open(".") == NULL);

	map = MapFile_open(UnitTest_tempname("data"));
	EXPECT_TRUE(map == NULL);

	TEST_FILE("data", TEST_DATA);
	map = MapFile_open(UnitTest_tempname("data"));
	PRINT_STR(rd_ident(map));
	EXPECT_TRUE(map != NULL);
	EXPECT_STREQ(map->data, TEST_DATA);
	EXPECT_TRUE(rd_deref(map) == NULL);
}
