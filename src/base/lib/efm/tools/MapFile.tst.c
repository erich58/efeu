#include <EFEU/UnitTest.h>
#include <EFEU/MapFile.h>
#include <EFEU/procenv.h>

#include <EFEU/utest/mstr.tst.h>

#define	TEST_DATA	"1234567890ABCDEFGHIJKLMNOPQRSTUVMXYZ\n"

TEST_SUITE
{
	SetProgName(UnitTest_name);

	MapFile *map;
	EXPECT(MapFile_open("/unknown file") == NULL);
	EXPECT(MapFile_open(".") == NULL);

	map = MapFile_open(UnitTest_tmp("data"));
	EXPECT(map == NULL);

	TEST_FILE("data", TEST_DATA);
	map = MapFile_open(UnitTest_tmp("data"));
	PRINT_MSTR(rd_ident(map));
	EXPECT(map != NULL);
	EXPECT_STR(map->data, TEST_DATA);
	EXPECT(rd_deref(map) == NULL);
	CLEANUP_MSTR;
}
