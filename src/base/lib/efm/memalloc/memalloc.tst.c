#include <EFEU/UnitTest.h>
#include <EFEU/Debug.h>

static void test_sizealign(void)
{
	EXPECT_TRUE(sizealign(0, 4) == 0);
	EXPECT_TRUE(sizealign(1, 4) == 4);
	EXPECT_TRUE(sizealign(4, 4) == 4);
	EXPECT_TRUE(sizealign(5, 4) == 8);
	EXPECT_TRUE(sizealign(123456, 1000) == 124000);
}

static void test_alloctab(void)
{
//	LogConfig("AllocTabTest.*=stderr");
	ALLOCTAB(tab, "AllocTabTest", 2, sizeof(int));
	EXPECT_STREQ(tab.name, "Test");
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
}

static void test_lmalloc(void)
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

TEST_SUITE
{
	test_sizealign();
	test_alloctab();
	test_lmalloc();
}
