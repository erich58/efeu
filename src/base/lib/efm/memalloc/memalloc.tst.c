#include <EFEU/UnitTest.h>
#include <EFEU/Debug.h>
#include <stdio.h>
#include <signal.h>

#include "utest/data.tst.h"

static ALLOCTAB(tab, "AllocTabTest", 2, sizeof(int));

static void activate_log(const char *log, const char *name)
{
	char buf[1024];
	snprintf(buf, sizeof buf, "%s=%s", log, UnitTest_tmpname(name));
	LogConfig(buf);
}

static void clear_log(const char *name)
{
	remove(UnitTest_tmpname(name));
}

static void sig_exit(int sig)
{
	exit(sig);
}

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
	EXPECT_STR(tab.name, "AllocTabTest");
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

	EXPECT_TRUE(new_data(NULL) == NULL);
	EXPECT_TRUE(del_data(NULL, NULL) == 1);
	EXPECT_TRUE(del_data(NULL, "x") == 0);

	AllocTab_stat(out(), &tab, 1);
	EXPECT_MATCH(data(), "^AllocTabTest");

	clear_log("error.log");
	EXPECT_TRUE(del_data(&tab, p) == 0);
	MATCH_CONTENT("error.log",
		"alloctab.AllocTabTest.: Pointer [^ ]* already released.\n");

}

TEST_DECL(lmalloc)
{
	char *p = NULL;

	EXPECT_TRUE((p = lrealloc(p, 32)) != NULL);
	PRINT_PTR(p);
	EXPECT_TRUE((p = lrealloc(p, 64)) != NULL);
	PRINT_PTR(p);
	EXPECT_TRUE((p = lrealloc(p, 0)) == NULL);
}

TEST_DECL(memalloc)
{
	char *p = NULL;
	EXPECT_TRUE((p = memalloc(8)) != NULL);
	memfree(p);
	memfree("a");
	memfree(NULL);

	TEST_FORK
	{
		int i;

		freopen("/dev/null", "w", stderr);
		memtrace = 1;
		memalloc(1024);

		for (i = 0; i < 2000; i++)
			lmalloc(32);

		memstat(NULL);
		meminfo(NULL);
		memchange(NULL);
		memcheck();
		lcheckall();
		TEST_EXIT;
	}

	TEST_WAIT;
}

TEST_DECL(memswap)
{
	char a[] = "123456789";
	char b[] = "ABCDEFGHI";
	memswap(a + 3, b + 3, 3);
	EXPECT_STR(a, "123DEF789");
	EXPECT_STR(b, "ABC456GHI");
}

TEST_DECL(debug_check)
{
	TEST_FILE("debug.log", NULL);
	del_data(&tab, new_data(&tab));

	TEST_FORK
	{
		signal(SIGABRT, sig_exit);
		tab.nfree++;
		new_data(&tab);
		TEST_EXIT;
	}

	CHILD_EXIT(6);
	MATCH_CONTENT("debug.log", "assignment parameters");

	TEST_FILE("debug.log", NULL);

	TEST_FORK
	{
		signal(SIGABRT, sig_exit);
		new_data(&tab);
		tab.nfree++;
		tab.nused--;
		new_data(&tab);
		TEST_EXIT;
	}

	CHILD_EXIT(6);
	MATCH_CONTENT("debug.log", "missing entries");

	TEST_FILE("debug.log", NULL);

	TEST_FORK
	{
		signal(SIGABRT, sig_exit);
		AllocTabList list = { tab.freelist->next };
		tab.freelist = &list;
		new_data(&tab);
		TEST_EXIT;
	}

	CHILD_EXIT(6);
	MATCH_CONTENT("debug.log", "not part of block list");
}

TEST_DECL(trace)
{
	activate_log("alloctab.trace", "trace.log");
	void *p = new_data(&tab );
	del_data(&tab, p);
	PRINT_STR(UnitTest_content("trace.log"));
	MATCH_CONTENT("trace.log",
		"alloctab.AllocTabTest.: Pointer [^ ]* allocated.\n"
		"alloctab.AllocTabTest.: Pointer [^ ]* released.\n");
}

TEST_SUITE
{
	activate_log(".err", "error.log");
	activate_log("alloctab.debug", "debug.log");
	TEST_RUN(sizealign);
	TEST_RUN(alloctab);
	TEST_RUN(lmalloc);
	TEST_RUN(memalloc);
	TEST_RUN(memswap);
	TEST_RUN(trace);
	TEST_RUN(debug_check);
	LogConfig("");
}
