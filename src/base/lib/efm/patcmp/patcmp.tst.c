#include <EFEU/UnitTest.h>
#include <EFEU/patcmp.h>
#include <EFEU/strbuf.h>
#include <EFEU/RegExp.h>
#include <EFEU/MatchPar.h>

#include "utest/mstr.tst.h"

TEST_DECL(patcmp)
{
	char *p = NULL;
	EXPECT(patcmp(NULL, NULL, NULL) == 1);
	EXPECT(patcmp(NULL, "", &p) == 0);
	EXPECT_STR(p, "");
	EXPECT(patcmp("!", NULL, NULL) == 1);
	EXPECT(patcmp("^", NULL, NULL) == 1);
	EXPECT(patcmp("!!", NULL, NULL) == 0);
	EXPECT(patcmp("a", "abc", NULL) == 0);
	EXPECT(patcmp("a", "abc", &p) == 1);
	EXPECT_STR(p, "bc");
	EXPECT(patcmp("!x", "xyz", &p) == 0);
	EXPECT_STR(p, "yz");
	EXPECT(patcmp("!xz", "xyz", &p) == 1);
	EXPECT_STR(p, "yz");

	EXPECT(patcmp("a?c", "abc", NULL) == 1);
	EXPECT(patcmp("a*c", "ac", NULL) == 1);
	EXPECT(patcmp("a*c", "a12345c", NULL) == 1);
	EXPECT(patcmp("a*", "a12345c", NULL) == 1);
	EXPECT(patcmp("a*x", "a12345c", NULL) == 0);

	EXPECT(patcmp("<3>a", "aa", NULL) == 0);
	EXPECT(patcmp("<-3>a", "aa", NULL) == 1);
	EXPECT(patcmp("<3>a", "aaa", NULL) == 1);
	EXPECT(patcmp("<3-2>a", "aa", NULL) == 1);
	EXPECT(patcmp("<3-2>a", "aaaa", NULL) == 0);
	EXPECT(patcmp("a<>0b", "ab", NULL) == 1);
	EXPECT(patcmp("a<>0b", "a0000000b", NULL) == 1);

	EXPECT(patcmp("a:x:b", "ab", NULL) == 1);
	EXPECT(patcmp("a:x:b", "axb", NULL) == 1);
	EXPECT(patcmp("a:x:b", "axxb", NULL) == 0);
	EXPECT(patcmp("a:x:b", "ayb", NULL) == 0);

	EXPECT(patcmp("x<2>{ab}y", "xababy", NULL) == 1);
	EXPECT(patcmp("x<2>{ab}y", "xaby", NULL) == 0);

	EXPECT(patcmp("x<>[0-9]y", "xy", NULL) == 1);
	EXPECT(patcmp("x<>[0-9]y", "x123y", NULL) == 1);
	EXPECT(patcmp("x<>[0-9]y", "x123a56y", NULL) == 0);
}


static const char *matchlist(const char *def, const char *data)
{
	static char buf[32];
	int n;

	for (n = 0; *data && n < 31; ++data)
		if (listcmp(def, *data))
			buf[n++] = *data;

	buf[n] = 0;
	return buf;
}

TEST_DECL(listcmp)
{
	EXPECT(listcmp(NULL, 0) == 0);
	EXPECT(listcmp("%@", 0) == 1);
	EXPECT_STR(matchlist("a", "abc"), "a");
	EXPECT_STR(matchlist("!a", "abc"), "bc");
	EXPECT_STR(matchlist("^a", "abc"), "bc");
	EXPECT_STR(matchlist("!!a", "abc"), "a");
	EXPECT_STR(matchlist("1-", "01234-"), "1-");
	EXPECT_STR(matchlist("1-3", "01234-"), "123");
	EXPECT_STR(matchlist("3-1", "01234-"), "123");
	EXPECT_STR(matchlist("1\\-3", "01234-"), "13-");
	EXPECT_STR(matchlist("1-\\3", "01234-"), "123");
	EXPECT_STR(matchlist("%", "abc3$%&"), "%");
	EXPECT_STR(matchlist("%%", "abc3$%&"), "%");
	EXPECT_STR(matchlist("\\%a", "abc3$%&"), "a%");
	EXPECT_STR(matchlist("%a", "abc3$%&"), "abc");
	EXPECT_STR(matchlist("%d", "0123456789abcdefgh"), "0123456789");
	EXPECT_STR(matchlist("%o", "0123456789abcdefgh"), "01234567");
	EXPECT_STR(matchlist("%x", "0123456789abcdefgh"), "0123456789abcdef");
	EXPECT_STR(matchlist("%s", "123 \t\n\r\fabc"), " \t\n\r\f");
	EXPECT_STR(matchlist("%u", "abcABC"), "ABC");
	EXPECT_STR(matchlist("%l", "abcABC"), "abc");
	EXPECT_STR(matchlist("%n", "abc.:; \t123"), "abc123");
	EXPECT_STR(matchlist("%p", "abc.:; \t123"), ".:;");
	EXPECT_STR(matchlist("%^", "\001\002\t\n !"), "\001\002\t\n");
	EXPECT_STR(matchlist("%~", "\176\177\200\201\202"), "\200\201\202");
}

TEST_DECL(pselect)
{
	char *list[] = { "+<>[%a]", "-a*", "a[0-9]" };
	EXPECT(patselect("xyz", list, 3) == 1);
	EXPECT(patselect("abc", list, 3) == 0);
	EXPECT(patselect("a2", list, 3) == 1);
}

TEST_DECL(cmp)
{
	EXPECT(alnumcmp(NULL, NULL) == 0);
	EXPECT(alnumcmp(NULL, "") < 0);
	EXPECT(alnumcmp("", NULL) > 0);
	EXPECT(alnumcmp("ab:12;-5", "a-b/12/5") == 0);
	EXPECT(alnumcmp("ab:12;-6", "a-b/12/5") > 0);

	EXPECT(alphacmp(NULL, NULL) == 0);
	EXPECT(alphacmp(NULL, "") < 0);
	EXPECT(alphacmp("", NULL) > 0);
	EXPECT(alphacmp("ab:12;-5x", "a-b/43x") == 0);
	EXPECT(alphacmp("ab:12;-5X", "a-b/43x") < 0);

	EXPECT(lexcmp(NULL, NULL) == 0);
	EXPECT(lexcmp(NULL, "") < 0);
	EXPECT(lexcmp("", NULL) > 0);
	EXPECT(lexcmp("abc12", "ABC\t12") == 0);
	EXPECT(lexcmp("abc12", "ABC 12") > 0);
	EXPECT(lexcmp("äöüß", "aeoeuess") == 0);
	EXPECT_STR(lexsortkey("ä.Ö.ü.ß.€.", NULL), "ae.oe.ue.ss..");
}

TEST_DECL(regexp)
{
	RegExp *r;
	regmatch_t *match;
	EXPECT(RegExp_comp(NULL, 0) == NULL);
	EXPECT_STR(RegExp_error, NULL);
	EXPECT(RegExp_comp("[x", 0) == NULL);
	MATCH_STR(RegExp_error, ".");

	EXPECT_PTR(r = RegExp_comp("[0-9]+", 1));
	EXPECT_MSTR(rd_ident(r), "/[0-9]+/i");
	EXPECT(RegExp_exec(r, "abc123xyz", &match) == 1);
	EXPECT(match[0].rm_so == 3);
	EXPECT(match[0].rm_eo == 6);
	EXPECT(RegExp_exec(r, "abc", NULL) == 0);
	EXPECT_MSTR(RegExp_subst(r, "[$$$0$]", "a2b45c", 0), "a[$2$]b45c");
	EXPECT_MSTR(RegExp_subst(r, "[$$$0$]", "a2b45c", 1), "a[$2$]b[$45$]c");
	EXPECT(rd_deref(r) == NULL);

	EXPECT_PTR(r = RegExp_comp(":?", 1));
	EXPECT_MSTR(RegExp_subst(r, "<$0>", "ab:c", 1), "a<>b<:><>c");
	EXPECT(rd_deref(r) == NULL);

	EXPECT(RegExp_exec(NULL, NULL, NULL) == 1);
	EXPECT(RegExp_exec(NULL, "", NULL) == 0);
}

TEST_DECL(mpar)
{
	MatchPar *m;

	EXPECT_PTR(m = MatchPar_create(NULL, 0));
	EXPECT_MSTR(rd_ident(m), "[<true>]");
	EXPECT(MatchPar_exec(m, NULL, 0) == 1);
	EXPECT(rd_deref(m) == NULL);

	EXPECT_PTR(m = MatchPar_create("#3:-1", 5));
	EXPECT_MSTR(rd_ident(m), "[#3:5]");
	EXPECT(MatchPar_exec(m, NULL, 2) == 0);
	EXPECT(MatchPar_exec(m, NULL, 3) == 1);
	EXPECT(MatchPar_exec(m, NULL, 6) == 0);
	EXPECT(rd_deref(m) == NULL);

	EXPECT_PTR(m = MatchPar_create("+#3:1", 5));
	EXPECT_MSTR(rd_ident(m), "[#3:1]");
	EXPECT(MatchPar_exec(m, NULL, 3) == 0);
	EXPECT(rd_deref(m) == NULL);

	EXPECT_PTR(m = MatchPar_create("-#2", 5));
	EXPECT_MSTR(rd_ident(m), "[-#2:2]");
	EXPECT(MatchPar_exec(m, NULL, 0) == 1);
	EXPECT(MatchPar_exec(m, NULL, 2) == 0);
	EXPECT(rd_deref(m) == NULL);

	EXPECT_PTR(m = MatchPar_create("/a/i", 0));
	EXPECT_MSTR(rd_ident(m), "[/a/i]");
	EXPECT(MatchPar_exec(m, "b", 0) == 0);
	EXPECT(MatchPar_exec(m, "bA", 0) == 1);
	EXPECT(rd_deref(m) == NULL);

	UnitTest_stderr("stderr");
	EXPECT_PTR(m = MatchPar_create("/[x/", 0));
	MATCH_CONTENT("stderr", ".");
	EXPECT_MSTR(rd_ident(m), "[<false>]");
	EXPECT(MatchPar_exec(m, "x", 0) == 0);
	EXPECT(rd_deref(m) == NULL);

	EXPECT_PTR(m = MatchPar_create("a*z", 0));
	EXPECT_MSTR(rd_ident(m), "[a*z]");
	EXPECT(MatchPar_exec(m, "abz", 0) == 1);
	EXPECT(MatchPar_exec(m, "bz", 0) == 0);
	EXPECT(rd_deref(m) == NULL);

	char *list[] = { "#2", "a*z" };
	EXPECT_PTR(m = MatchPar_vec(list, 2, 5));
	EXPECT_MSTR(rd_ident(m), "[#2:2 a*z]");
	EXPECT(MatchPar_exec(m, "x", 0) == 0);
	EXPECT(MatchPar_exec(m, "x", 2) == 1);
	EXPECT(MatchPar_exec(m, "az", 0) == 1);
	EXPECT(rd_deref(m) == NULL);

	EXPECT_PTR(m = MatchPar_list("#-2 b[ ]\\,c,\t /a/, \\x\\", 5));
	EXPECT_MSTR(rd_ident(m), "[#4:4 b[ ],c /a/ \\x\\]");
	EXPECT(MatchPar_exec(m, "x", 0) == 1);
	EXPECT(rd_deref(m) == NULL);

	EXPECT(MatchPar_exec(NULL, NULL, 0) == 0);
}


TEST_SUITE
{
	TEST_RUN(patcmp);
	TEST_RUN(listcmp);
	TEST_RUN(pselect);
	TEST_RUN(cmp);
	TEST_RUN(regexp);
	TEST_RUN(mpar);
	CLEANUP_MSTR;
}
