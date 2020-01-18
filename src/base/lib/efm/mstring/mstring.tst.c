#include <EFEU/UnitTest.h>
#include <EFEU/mstring.h>
#include <stdio.h>
#include <signal.h>

//#include "utest/data.tst.h"

TEST_DECL(mbasename)
{
	char *b, *s;
	EXPECT_STREQ(mdirname(NULL, 0), NULL);
	EXPECT_STREQ(mdirname("abc", 0), NULL);
	EXPECT_STREQ(mdirname("///", 0), "/");
	EXPECT_STREQ(mdirname("/a", 0), "/");
	EXPECT_STREQ(mdirname("a/b//", 0), "a");
	EXPECT_STREQ(mdirname("a/b/c", 1), "a/b/");

	EXPECT_STREQ(mbasename(NULL, NULL), NULL);
	EXPECT_STREQ((b = mbasename("a/b.c", NULL)), "b.c");
	EXPECT_STREQ((b = mbasename("/b.c", &s)), "b");
	EXPECT_STREQ(s, "c");
	EXPECT_TRUE(b + strlen(b) + 1 == s);
}

TEST_DECL(mstrcmp)
{
	char a[] = "a";
	EXPECT_TRUE(mstrcmp(NULL, NULL) == 0);
	EXPECT_TRUE(mstrcmp("", NULL) > 0);
	EXPECT_TRUE(mstrcmp(NULL, "") < 0);
	PRINT_STR(a);
	EXPECT_TRUE(mstrcmp(a, "a") == 0);
	EXPECT_TRUE(mstrcmp(a, "x") < 0);
	EXPECT_TRUE(mstrcmp("aa", a) > 0);
	EXPECT_TRUE(mstrlen(NULL) == 0);
	EXPECT_TRUE(mstrlen("abc") == 3);
}

TEST_DECL(mtestkey)
{
	char *p;
	EXPECT_TRUE(mtestkey(NULL, NULL, NULL));
	EXPECT_FALSE(mtestkey(NULL, NULL, ""));
	EXPECT_FALSE(mtestkey("abc", &p, NULL));
	EXPECT_STREQ(p, "abc");
	EXPECT_TRUE(mtestkey(" abc xyz", &p, "a?c"));
	EXPECT_STREQ(p, " xyz");
	EXPECT_TRUE(mtestkey("\t abc", NULL, "ab:c"));
	EXPECT_TRUE(mtestkey("ab", NULL, "ab:c"));
	EXPECT_FALSE(mtestkey("a", NULL, "ab:c"));
	EXPECT_FALSE(mtestkey("abx", NULL, "ab:c"));
	EXPECT_TRUE(mtestkey("abxyz", NULL, "ab*"));
	EXPECT_TRUE(mtestkey(" _a123_(xyz)", &p, "*"));
	EXPECT_STREQ(p, "(xyz)");
}

TEST_DECL(mtrim)
{
	char p[] = " \t\nxyz abc \t\n";
	TEST_EXPR(mtrim(NULL));
	PRINT_STR(p);
	TEST_EXPR(mtrim(p));
	EXPECT_STREQ(p, "xyz abc");
}

TEST_DECL(mstrcpy)
{
	EXPECT_STREQ(mstrcpy(NULL), NULL);
	EXPECT_STREQ(mstrcpy("abc"), "abc");
	EXPECT_STREQ(mstrncpy(NULL, 5), "");
	EXPECT_STREQ(mstrncpy("abc", 5), "abc");
	EXPECT_STREQ(mstrncpy("abc", 2), "ab");
}

TEST_DECL(mlangcpy)
{
	EXPECT_STREQ(mlangcpy(":de:ABC:*:xyz", "de_AT"), "ABC");
	EXPECT_STREQ(mlangcpy(":de:ABC:*:xyz", NULL), "xyz");
	EXPECT_STREQ(mlangcpy(NULL, NULL), NULL);
}

TEST_DECL(mstrcat)
{
	EXPECT_STREQ(mstrpaste(".", NULL, NULL), NULL);
	EXPECT_STREQ(mstrpaste(".", "x", NULL), "x");
	EXPECT_STREQ(mstrpaste(".", NULL, "y"), "y");
	EXPECT_STREQ(mstrpaste(".", "x", "y"), "x.y");
	EXPECT_STREQ(mstrpaste(NULL, "x", "y"), "xy");

	EXPECT_STREQ(mstrcat(".", NULL), NULL);
	EXPECT_STREQ(mstrcat(".", "x", NULL), "x");
	EXPECT_STREQ(mstrcat(".", "x", "y", NULL), "x.y");
	EXPECT_STREQ(mstrcat(NULL, "x", "y", NULL), "xy");
}

TEST_DECL(mtabcat)
{
	char *tab[] = { "a", "b", "c" };

	EXPECT_STREQ(mtabcat(NULL, tab, 3), "abc");
	EXPECT_STREQ(mtabcat(".", tab, 3), "a.b.c");
	EXPECT_STREQ(mtabcat(".", NULL, 3), NULL);
	EXPECT_STREQ(mtabcat(".", tab, 0), NULL);
}

TEST_DECL(mstrcut)
{
	char *p;
	EXPECT_STREQ(mstrcut(NULL, NULL, NULL, 0), NULL);
	EXPECT_STREQ(mstrcut("ab cd", &p, NULL, 0), "ab");
	EXPECT_STREQ(p, "cd");
	EXPECT_STREQ(mstrcut("ab\\ cd", &p, NULL, 0), "ab\\ cd");
	EXPECT_STREQ(p, NULL);
	EXPECT_STREQ(mstrcut("ab\tcd", &p, NULL, 0), "ab");
	EXPECT_STREQ(p, "cd");
	EXPECT_STREQ(mstrcut("ab  cd", &p, NULL, 0), "ab");
	EXPECT_STREQ(p, " cd");
	EXPECT_STREQ(mstrcut("ab \t  cd", &p, NULL, 1), "ab");
	EXPECT_STREQ(p, "cd");
	EXPECT_STREQ(mstrcut("ab.:;cd", &p, ".:;", 1), "ab");
	EXPECT_STREQ(p, "cd");
}

TEST_DECL(mstrsplit)
{
	char s[] = "abc\0xyz\0\0rest";
	char *p = s;
	EXPECT_STREQ(nextstr(&p), "abc");
	EXPECT_STREQ(nextstr(&p), "xyz");
	EXPECT_STREQ(nextstr(&p), NULL);
	p = NULL;
	EXPECT_STREQ(nextstr(&p), NULL);

	char **tab;
	EXPECT_TRUE(mstrsplit(NULL, "%s,", &tab) == 0);
	EXPECT_TRUE(mstrsplit("a b", "%s,", NULL) == 0);
	EXPECT_TRUE(mstrsplit(" , a b , , c", "%s,", &tab) == 5);
	EXPECT_STREQ(tab[0], "");
	EXPECT_STREQ(tab[1], "a");
	EXPECT_STREQ(tab[2], "b");
	EXPECT_STREQ(tab[3], "");
	EXPECT_STREQ(tab[4], "c");
}

TEST_DECL(msprintf)
{
	va_list args;
	EXPECT_STREQ(msprintf(NULL), NULL);
	EXPECT_STREQ(mvsprintf(NULL, args), NULL);
	EXPECT_STREQ(msprintf("%d", 5), "5");
}

TEST_SUITE
{
	TEST_RUN(mbasename);
	TEST_RUN(mstrcmp);
	TEST_RUN(mtestkey);
	TEST_RUN(mtrim);
	TEST_RUN(mstrcpy);
	TEST_RUN(mlangcpy);
	TEST_RUN(mstrcat);
	TEST_RUN(mtabcat);
	TEST_RUN(mstrcut);
	TEST_RUN(mstrsplit);
	TEST_RUN(msprintf);
}
