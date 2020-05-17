#include <EFEU/UnitTest.h>
#include <EFEU/mstring.h>
#include <stdio.h>
#include <signal.h>

TEST_DECL(mbasename)
{
	char *b, *s;
	EXPECT_STR(mdirname(NULL, 0), NULL);
	EXPECT_STR(mdirname("abc", 0), NULL);
	EXPECT_STR(mdirname("///", 0), "/");
	EXPECT_STR(mdirname("/a", 0), "/");
	EXPECT_STR(mdirname("a/b//", 0), "a");
	EXPECT_STR(mdirname("a/b/c", 1), "a/b/");

	EXPECT_STR(mbasename(NULL, NULL), NULL);
	EXPECT_STR((b = mbasename("a/b.c", NULL)), "b.c");
	EXPECT_STR((b = mbasename("/b.c", &s)), "b");
	EXPECT_STR(s, "c");
	EXPECT(b + strlen(b) + 1 == s);
}

TEST_DECL(mstrcmp)
{
	char a[] = "a";
	EXPECT(mstrcmp(NULL, NULL) == 0);
	EXPECT(mstrcmp("", NULL) > 0);
	EXPECT(mstrcmp(NULL, "") < 0);
	PRINT_STR(a);
	EXPECT(mstrcmp(a, "a") == 0);
	EXPECT(mstrcmp(a, "x") < 0);
	EXPECT(mstrcmp("aa", a) > 0);
	EXPECT(mstrlen(NULL) == 0);
	EXPECT(mstrlen("abc") == 3);
}

TEST_DECL(mtestkey)
{
	char *p;
	EXPECT(mtestkey(NULL, NULL, NULL));
	EXPECT(!mtestkey(NULL, NULL, ""));
	EXPECT(!mtestkey("abc", &p, NULL));
	EXPECT_STR(p, "abc");
	EXPECT(mtestkey(" abc xyz", &p, "a?c"));
	EXPECT_STR(p, " xyz");
	EXPECT(mtestkey("\t abc", NULL, "ab:c"));
	EXPECT(mtestkey("ab", NULL, "ab:c"));
	EXPECT(!mtestkey("a", NULL, "ab:c"));
	EXPECT(!mtestkey("abx", NULL, "ab:c"));
	EXPECT(mtestkey("abxyz", NULL, "ab*"));
	EXPECT(mtestkey(" _a123_(xyz)", &p, "*"));
	EXPECT_STR(p, "(xyz)");
}

TEST_DECL(mtrim)
{
	char p[] = " \t\nxyz abc \t\n";
	TEST_EXPR(mtrim(NULL));
	PRINT_STR(p);
	TEST_EXPR(mtrim(p));
	EXPECT_STR(p, "xyz abc");
}

TEST_DECL(mstrcpy)
{
	EXPECT_STR(mstrcpy(NULL), NULL);
	EXPECT_STR(mstrcpy("abc"), "abc");
	EXPECT_STR(mstrncpy(NULL, 5), "");
	EXPECT_STR(mstrncpy("abc", 5), "abc");
	EXPECT_STR(mstrncpy("abc", 2), "ab");
}

TEST_DECL(mlangcpy)
{
	EXPECT_STR(mlangcpy(":de:ABC:*:xyz", "de_AT"), "ABC");
	EXPECT_STR(mlangcpy(":de:ABC:*:xyz", NULL), "xyz");
	EXPECT_STR(mlangcpy(":de:ABC:en:xyz", NULL), NULL);
	EXPECT_STR(mlangcpy(NULL, NULL), NULL);
}

TEST_DECL(mstrcat)
{
	EXPECT_STR(mstrpaste(".", NULL, NULL), NULL);
	EXPECT_STR(mstrpaste(".", "x", NULL), "x");
	EXPECT_STR(mstrpaste(".", NULL, "y"), "y");
	EXPECT_STR(mstrpaste(".", "x", "y"), "x.y");
	EXPECT_STR(mstrpaste(NULL, "x", "y"), "xy");

	EXPECT_STR(mstrcat(".", NULL), NULL);
	EXPECT_STR(mstrcat(".", "x", NULL), "x");
	EXPECT_STR(mstrcat(".", "x", "y", NULL), "x.y");
	EXPECT_STR(mstrcat(NULL, "x", "y", NULL), "xy");
}

TEST_DECL(mtabcat)
{
	char *tab[] = { "a", "b", "c" };

	EXPECT_STR(mtabcat(NULL, tab, 3), "abc");
	EXPECT_STR(mtabcat(".", tab, 3), "a.b.c");
	EXPECT_STR(mtabcat(".", NULL, 3), NULL);
	EXPECT_STR(mtabcat(".", tab, 0), NULL);
}

TEST_DECL(mstrcut)
{
	char *p;
	EXPECT_STR(mstrcut(NULL, NULL, NULL, 0), NULL);
	EXPECT_STR(mstrcut("ab cd", &p, NULL, 0), "ab");
	EXPECT_STR(p, "cd");
	EXPECT_STR(mstrcut("ab\\ cd", &p, NULL, 0), "ab\\ cd");
	EXPECT_STR(p, NULL);
	EXPECT_STR(mstrcut("ab\tcd", &p, NULL, 0), "ab");
	EXPECT_STR(p, "cd");
	EXPECT_STR(mstrcut("ab  cd", &p, NULL, 0), "ab");
	EXPECT_STR(p, " cd");
	EXPECT_STR(mstrcut("ab \t  cd", &p, NULL, 1), "ab");
	EXPECT_STR(p, "cd");
	EXPECT_STR(mstrcut("ab.:;cd", &p, ".:;", 1), "ab");
	EXPECT_STR(p, "cd");
}

TEST_DECL(mstrsplit)
{
	char s[] = "abc\0xyz\0\0rest";
	char *p = s;
	EXPECT_STR(nextstr(&p), "abc");
	EXPECT_STR(nextstr(&p), "xyz");
	EXPECT_STR(nextstr(&p), NULL);
	p = NULL;
	EXPECT_STR(nextstr(&p), NULL);

	char **tab;
	EXPECT(mstrsplit(NULL, "%s,", &tab) == 0);
	EXPECT(mstrsplit("a b", "%s,", NULL) == 0);
	EXPECT(mstrsplit(" , a b , , c", "%s,", &tab) == 5);
	EXPECT_STR(tab[0], "");
	EXPECT_STR(tab[1], "a");
	EXPECT_STR(tab[2], "b");
	EXPECT_STR(tab[3], "");
	EXPECT_STR(tab[4], "c");
}

TEST_DECL(msprintf)
{
	va_list args;
	EXPECT_STR(msprintf(NULL), NULL);
	EXPECT_STR(mvsprintf(NULL, args), NULL);
	EXPECT_STR(msprintf("%d", 5), "5");
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
