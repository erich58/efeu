#include <EFEU/UnitTest.h>
#include <EFEU/mkpath.h>
#include <EFEU/mstring.h>

TEST_DECL(mkpath)
{
	STRBUF(buf, 256);
	EXPECT_TRUE(mkpath_bin(&buf, "bin", "lib", "else", NULL));
	EXPECT_STREQ(sb_nul(&buf), "lib:else");
	sb_clean(&buf);
	EXPECT_FALSE(mkpath_bin(&buf, "x", "lib", "else", NULL));
	EXPECT_STREQ(sb_nul(&buf), "");
	EXPECT_FALSE(mkpath_bin(&buf, "bin32", "lib", "else", NULL));
	EXPECT_STREQ(sb_nul(&buf), "");
	EXPECT_TRUE(mkpath_bin(&buf, "x/bin", "lib", "else", NULL));
	EXPECT_STREQ(sb_nul(&buf), "x/lib:x/else");

	char *path = mstrcpy(getenv("PATH")); /* Save PATH */
	TEST_EXPR(setenv("PATH", "a/bin:b/bin", 1));

	sb_clean(&buf);
	TEST_EXPR(mkpath_base(&buf, "lib", "else", NULL));
	EXPECT_STREQ(sb_nul(&buf), "a/lib:a/else:b/lib:b/else");

	char *p;
	TEST_EXPR(setenv("TESTENV", "x:y", 1));
	TEST_EXPR(p = mkpath("z/bin", "TESTENV", "s", "t", NULL));
	EXPECT_STREQ(p, "z/s:z/t:x:y:a/s:a/t:b/s:b/t");

	memfree(p);
	TEST_EXPR(setenv("HOME", "/home/test", 1));
	TEST_EXPR(p = ExpandPath("/bin:~/bin:~root/bin"));
	EXPECT_STREQ(p, "/bin:/home/test/bin:/root/bin");

	setenv("PATH", path, 1);	/* Restore PATH */
}

static int c_flags = 0;

static void c_init()
{
	c_flags |= 1;
}

static void c_exit()
{
	c_flags |= 2;
}

TEST_DECL(callproc)
{
	Shell = "true";
	EXPECT_TRUE(callproc(NULL) == 0);
	Shell = NULL;
	EXPECT_TRUE(callproc(NULL) == -1);
	callproc_init = c_init;
	callproc_exit = c_exit;
	EXPECT_TRUE(callproc("exit 5") == 5);
	EXPECT_TRUE(c_flags == 3);
	TEST_FILE("x.sh", "echo TEST\n");
}

TEST_SUITE
{
	TEST_RUN(mkpath);
	TEST_RUN(callproc);
}
