#include <EFEU/UnitTest.h>
#include <EFEU/mkpath.h>
#include <EFEU/mstring.h>

TEST_SUITE
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
