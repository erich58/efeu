#include <EFEU/UnitTest.h>
#include <EFEU/Debug.h>
#include <EFEU/mstring.h>

static ALLOCTAB(tab, "AllocTabTest", 2, sizeof(int));

TEST_DECL(sizealign)
{
	EXPECT_TRUE(sizealign(0, 4) == 0);
	EXPECT_TRUE(sizealign(1, 4) == 4);
	EXPECT_TRUE(sizealign(4, 4) == 4);
	EXPECT_TRUE(sizealign(5, 4) == 8);
	EXPECT_TRUE(sizealign(123456, 1000) == 124000);
}

TEST_DECL(alloctab)
{
	EXPECT_STREQ(tab.name, "AllocTabTest");
	EXPECT_TRUE(tab.blksize == 2);
	EXPECT_TRUE(tab.elsize == sizeof(AllocTabList));
	EXPECT_TRUE(tab.nfree == 0);
	EXPECT_TRUE(tab.nused == 0);
	EXPECT_TRUE(tab.blklist == NULL);
	EXPECT_TRUE(tab.freelist == NULL);
	EXPECT_TRUE(tab.chain == NULL);
	void *p;
	EXPECT_TRUE((p = new_data(&tab)) != NULL);
	EXPECT_TRUE(tab.nfree == 1);
	EXPECT_TRUE(tab.nused == 1);

	if	(EXPECT_TRUE(tab.blklist != NULL))
		EXPECT_TRUE(tab.blklist->next == NULL);

	if	(EXPECT_TRUE(tab.freelist != NULL))
		EXPECT_TRUE(tab.freelist->next == NULL);

	EXPECT_TRUE(tab.chain != NULL);
	EXPECT_TRUE(del_data(&tab, "x") == 0);
	EXPECT_TRUE(tab.nfree == 1);
	EXPECT_TRUE(tab.nused == 1);
	EXPECT_TRUE(del_data(&tab, p) == 1);
	EXPECT_TRUE(tab.freelist == p);
	EXPECT_TRUE(tab.nfree == 2);
	EXPECT_TRUE(tab.nused == 0);
	AllocTab_stat(ioerr, &tab, 1);

	PRINT_STR(UnitTest_content("debug.out"));
	PRINT_STR(UnitTest_content("trace.out"));
}

TEST_DECL(lmalloc)
{
	char *p = NULL;

	memtrace = 1;
	EXPECT_TRUE((p = lrealloc(p, 32)) != NULL);
	PRINT_PTR(p);
	meminfo(NULL);
	EXPECT_TRUE((p = lrealloc(p, 64)) != NULL);
	PRINT_PTR(p);
	meminfo(NULL);
	EXPECT_TRUE((p = lrealloc(p, 0)) == NULL);
	lcheckall();
}

TEST_DECL(memswap)
{
	char a[] = "123456789";
	char b[] = "ABCDEFGHI";
	memswap(a + 3, b + 3, 3);
	EXPECT_STREQ(a, "123DEF789");
	EXPECT_STREQ(b, "ABC456GHI");
}

TEST_DECL(Trace)
{
	char *cfg;
	cfg = msprintf("alloctab.trace=%s", UnitTest_tempname("trace.out"));
	LogConfig(cfg);
	memfree(cfg);
	cfg = msprintf("alloctab.debug=%s", UnitTest_tempname("debug.out"));
	LogConfig(cfg);
	memfree(cfg);
}

TEST_SUITE
{
	TEST_RUN(sizealign);
	TEST_RUN(alloctab);
	TEST_RUN(lmalloc);
	TEST_RUN(memswap);
}
