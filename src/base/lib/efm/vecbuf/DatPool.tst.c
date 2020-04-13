#include <EFEU/UnitTest.h>
#include <EFEU/DatPool.h>
#include <EFEU/memalloc.h>

#include "utest/mstr.tst.h"

TEST_SUITE
{
	DatPool *d = DatPool_dyn(4);
	EXPECT(d != NULL);
	EXPECT_MSTR(rd_ident(d), "0@0 + 0@0/4");
	EXPECT(DatPool_add(d, "abc", 3) == 0);
	EXPECT(DatPool_add(d, "0123456789", 10) == 3);
	EXPECT(DatPool_add(d, "xyz", 4) == 13);
	EXPECT_STR(DatPool_get(d, 0), "abc0123456789xyz");
	EXPECT_MSTR(rd_ident(d), "0@0 + 17@20/4");
	DatPool_create(d, UnitTest_tmpname("data"));
	rd_deref(d);

	d = DatPool_map(UnitTest_tmpname("data"));
	EXPECT_MSTR(rd_ident(d), "0@17 + 0@0/0");
	EXPECT(DatPool_add(d, "ABC", 4) == 17);
	EXPECT_STR(DatPool_get(d, 0), "abc0123456789xyz");
	EXPECT_STR(DatPool_get(d, 17), "ABC");
	EXPECT_MSTR(rd_ident(d), "0@17 + 4@100000/0");
	EXPECT_STR(DatPool_get(d, 16), "");
	EXPECT((char *)DatPool_get(d, 15) + 1 == DatPool_get(d, 16));
	EXPECT((char *)DatPool_get(d, 16) + 1 != DatPool_get(d, 17));
	EXPECT((char *)DatPool_get(d, 17) + 1 == DatPool_get(d, 18));
	EXPECT_STR(DatPool_get(d, 19), "C");
	EXPECT_STR(DatPool_get(d, 20), "");
	EXPECT_STR(DatPool_get(d, 21), NULL);
	EXPECT_STR(DatPool_get(NULL, 0), NULL);
	DatPool_create(d, UnitTest_tmpname("data.tmp"));
	rd_deref(d);

	system(UnitTest_tpl("mv @/data.tmp @/data"));
	const char *p = UnitTest_content("data");
	EXPECT_STR(p, "DatPool");
	EXPECT_STR(p + 8, "abc0123456789xyz");
	EXPECT_STR(p + 8 + 17, "ABC");

	d = DatPool_map(UnitTest_tmpname("data"));
	EXPECT_MSTR(rd_ident(d), "0@21 + 0@0/0");
	rd_deref(d);
	CLEANUP_MSTR;
}
