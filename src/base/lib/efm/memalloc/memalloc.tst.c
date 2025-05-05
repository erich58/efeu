#include <EFEU/UnitTest.h>
#include <EFEU/Debug.h>
#include <stdio.h>
#include <signal.h>

#include "utest/out.tst.h"

static ALLOCTAB(tab, "AllocTabTest", 2, sizeof(int));
static ALLOCTAB(unnamed, NULL, 0, 512);

static size_t ldim(void **list)
{
	size_t n = 0;

	while (list)
	{
		list = *list;
		n++;
	}

	return n;
}

static const char *status()
{
	static char buf[1024];
	snprintf(buf, sizeof buf, "blk=%zd seg=%zd used=%zd free=%zd",
		ldim(tab.blklist), ldim(tab.freelist), tab.nused, tab.nfree);
	return buf;
}


static void activate_log(const char *log, const char *name)
{
	char buf[1024];
	snprintf(buf, sizeof buf, "%s=%s", log, UnitTest_tmp(name));
	LogConfig(buf);
}

static void clear_log(const char *name)
{
	remove(UnitTest_tmp(name));
}

static void sig_exit(int sig)
{
	exit(sig);
}

TEST_DECL(sizealign)
{
	EXPECT(sizealign(0, 4) == 0);
	EXPECT(sizealign(1, 4) == 4);
	EXPECT(sizealign(4, 4) == 4);
	EXPECT(sizealign(5, 4) == 8);
	EXPECT(sizealign(123456, 1000) == 124000);
}

TEST_DECL(alloctab)
{
	EXPECT_STR(tab.name, "AllocTabTest");
	EXPECT(tab.blksize == 2);
	EXPECT(tab.elsize == sizeof(void *));
	EXPECT(tab.nfree == 0);
	EXPECT(tab.nused == 0);
	EXPECT(tab.blklist == NULL);
	EXPECT(tab.freelist == NULL);
	EXPECT(tab.chain == NULL);
	EXPECT_STR(status(), "blk=0 seg=0 used=0 free=0");

	void *p1;
	EXPECT((p1 = new_data(&tab)) != NULL);
	EXPECT(tab.chain != NULL);
	EXPECT_STR(status(), "blk=1 seg=1 used=1 free=1");

	void *p2;
	EXPECT((p2 = new_data(&tab)) != NULL);
	EXPECT_STR(status(), "blk=1 seg=0 used=2 free=0");

	void *p3;
	EXPECT((p3 = new_data(&tab)) != NULL);
	EXPECT_STR(status(), "blk=2 seg=1 used=3 free=1");

	EXPECT(del_data(&tab, p2) == 1);
	EXPECT(del_data(&tab, p1) == 1);
	EXPECT(del_data(&tab, p3) == 1);
	EXPECT_STR(status(), "blk=2 seg=4 used=0 free=4");

	EXPECT((p1 = new_data(&tab)) != NULL);
	EXPECT_STR(status(), "blk=2 seg=3 used=1 free=3");

	EXPECT(del_data(&tab, "x") == 0);
	EXPECT_STR(status(), "blk=2 seg=3 used=1 free=3");

	EXPECT(new_data(NULL) == NULL);
	EXPECT(del_data(NULL, NULL) == 1);
	EXPECT(del_data(NULL, "x") == 0);

	AllocTab_stat(OUT(), &tab, 1);
	MATCH_OUT("^AllocTabTest");

	clear_log("error.log");
	EXPECT(del_data(&tab, p3) == 0);
	MATCH_CONTENT("error.log",
		"alloctab.AllocTabTest.: Pointer [^ ]* already released.\n");

	clear_log("error.log");

	TEST_FORK
	{
		char *p = new_data(&tab);
		tst_data(&tab, p + 1); exit(0);
	}

	CHILD_EXIT(1);
	MATCH_CONTENT("error.log", "not a legal address");
}

TEST_DECL(memalloc)
{
	char *p = NULL;
	EXPECT((p = memalloc(8)) != NULL);
	memfree(p);
	memfree("a");
	memfree(NULL);

	TEST_FORK
	{
		int i;

		freopen("/dev/null", "w", stderr);
		memalloc(1024);

		for (i = 0; i < 2000; i++)
			lmalloc(32);

		memstat(NULL);
		memcheck();
		TEST_EXIT;
	}

	CHILD_EXIT(0);
}

TEST_DECL(memnotice)
{
	void *p = lmalloc(32);
	memnotice(p);

	UnitTest_stderr("stderr");
	memnotice(p);
	UnitTest_stderr(NULL);
	MATCH_CONTENT("stderr", "reused without freeing");

	memfree(p);
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
		new_data(&tab); exit(0);
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
		new_data(&tab); exit(0);
	}

	CHILD_EXIT(6);
	MATCH_CONTENT("debug.log", "missing entries");

	TEST_FILE("debug.log", NULL);

	TEST_FORK
	{
		signal(SIGABRT, sig_exit);
		new_data(&tab);
		tab.nfree--;
		tab.nused++;
		new_data(&tab); exit(0);
	}

	CHILD_EXIT(6);
	MATCH_CONTENT("debug.log", "too many entries");
	TEST_FILE("debug.log", NULL);

	TEST_FORK
	{
		signal(SIGABRT, sig_exit);
		void *list = tab.freelist;
		tab.freelist = &list;
		new_data(&tab); exit(0);
	}

	CHILD_EXIT(6);
	MATCH_CONTENT("debug.log", "not part of block list");

}

TEST_DECL(trace)
{
	activate_log("alloctab.trace", "trace.log");
	del_data(&tab, new_data(&tab));
	del_data(&unnamed, new_data(&unnamed));
	PRINT_STR(UnitTest_content("trace.log"));
	LMATCH_STR(UnitTest_content("trace.log"),
		"alloctab.AllocTabTest.: Pointer [^ ]* allocated.\n"
		"alloctab.AllocTabTest.: Pointer [^ ]* released.\n"
		"alloctab.0x.*: Pointer [^ ]* allocated.\n"
		"alloctab.0x.*: Pointer [^ ]* released.\n");
}

TEST_SUITE
{
	activate_log("*.err", "error.log");
	activate_log("alloctab.debug", "debug.log");
	TEST_RUN(sizealign);
	TEST_RUN(alloctab);
	TEST_RUN(memalloc);
	TEST_RUN(memnotice);
	TEST_RUN(memswap);
	TEST_RUN(trace);
	TEST_RUN(debug_check);
	LogConfig("");
	CLEANUP_OUT;
}
