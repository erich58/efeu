#!/usr/bin/env .build/script.tst

EXPECT_REPR(typeof '@', char);
EXPECT_REPR(typeof 'ä', char);
EXPECT_REPR(typeof '€', wchar_t);
EXPECT_REPR(typeof L'@', wchar_t);
EXPECT_REPR(typeof L'ä', wchar_t);

EXPECT((int) '@' == 64);
EXPECT((int) '€' == 8364);
EXPECT((int32_t) '€' == 8364);
EXPECT_REPR((char) 64, "@");
EXPECT_REPR((wchar_t) 8364, "€");

EXPECT_STR(str('ä'), "\344");
EXPECT_STR(str(L'ä'), "\303\244");

EXPECT(int("12") == 12);
EXPECT(int("12", 16) == 0x12);
EXPECT(int("0x12") == 0x12);
EXPECT(int("0x12", 10) == 0);
EXPECT(unsigned("-1") == ~0u);
EXPECT(unsigned("10101", 2) == 0b10101);
EXPECT(int32_t(123456, 10) == 123456);
EXPECT(uint32_t(123456, 10) == 123456);
EXPECT(int64_t(123456, 10) == 123456);
EXPECT(varint(123456, 10) == 123456);
EXPECT(uint64_t(123456, 10) == 123456);
EXPECT(varsize(123456, 10) == 123456);

EXPECT_DBL(double("12.5"), 12.5);

TEST_EXPR(str s = "12.5rest");
EXPECT(dstrcut(s) == 12.5);
EXPECT_STR(s, "rest");
TEST_EXPR(s = NULL);
EXPECT(dstrcut(s) == 0.0);

EXPECT_STR(str(NULL), NULL);
EXPECT_STR(str(1:2), "{1, 2}");
EXPECT_STR((str)'@', "@");
EXPECT_STR(str('@', 2), "@@");
EXPECT_STR(str(2.5), "2.5000");

EXPECT("a" < "b");
EXPECT("a" <= "b");
EXPECT("a" == "a");
EXPECT("a" != "b");
EXPECT("b" >= "a");
EXPECT("b" > "a");

EXPECT_STR("ab" * 2, "abab"); 
EXPECT_STR(3 * "ab", "ababab"); 
EXPECT_STR("a" + "b", "ab"); 
EXPECT_STR("5" + 1, "51"); 
EXPECT_STR(1 + "5", "15"); 

EXPECT_STR(paste("/", "a", "b"), "a/b");
EXPECT_STR(paste("/", "a", NULL), "a");
EXPECT_STR(paste("/", NULL, "b"), "b");
EXPECT_STR(paste(NULL, "a", "b"), "ab");
EXPECT(strlen("abc") == 3);
EXPECT_STR(langcpy(":de:deutsch:*:german", "de"), "deutsch");
EXPECT_STR(langcpy(":de:deutsch:*:german", "C"), "german");
EXPECT("abc"[1] == 'b');
EXPECT("abc"[5] == 0);
EXPECT_STR(substr("abc", 1), "bc");
EXPECT_STR(substr("abc", 1, 1), "b");
EXPECT_STR(substr("abc", 5, 1), NULL);
EXPECT_STR(substr("abc", 2, 5), "c");

TEST_EXPR(s = NULL);
EXPECT_STR(strcut(s, ","), NULL); 

TEST_EXPR(s = "a\"bc,,x\"yz");
EXPECT_STR(strcut(s, ",", 1), "a\"bc"); 
EXPECT_STR(s, "x\"yz"); 

TEST_EXPR(s = "abc,,xyz");
EXPECT_STR(strcut(s, ",", 0), "abc"); 
EXPECT_STR(s, ",xyz"); 

TEST_EXPR(s = "abc,,xyz");
EXPECT_STR(strcut(s, ",", -1), "abc"); 
EXPECT_STR(s, ",,xyz"); 

TEST_EXPR(s = "abc,,xyz");
EXPECT_STR(strcut(s, ": ", -1), "abc,,xyz"); 
EXPECT_STR(s, NULL); 

TEST_EXPR(s = NULL);
EXPECT_STR(xstrcut(s, ","), NULL); 

TEST_EXPR(s = "a\"bc,,x\"yz");
EXPECT_STR(xstrcut(s, ",", 1), "abc,,xyz"); 
EXPECT_STR(s, NULL); 

TEST_EXPR(s = "a'b\\nb'\"c\\nc\"\\,,xyz");
EXPECT_STR(xstrcut(s, ",", -1), "ab\\nbc\nc,"); 
EXPECT_STR(s, ",xyz"); 

EXPECT_STR(strsub("abcd,abcd", "bc", "-"), "a-d,a-d");
EXPECT_STR(strsub("abcd,abcd", "bc", "-", false), "a-d,abcd");
EXPECT_STR(strsub("abcd,abcd", "bc"), "ad,ad");
EXPECT_STR(strsub("abcd,abcd", NULL), "abcd,abcd");
EXPECT_STR(strsub("abcd", ""), NULL);
EXPECT_STR(strsub("abcd", "", "-"), "----");
EXPECT_STR(strsub("abcd", "", "-", false), "-bcd");
EXPECT_STR(strsub(NULL, "", "x"), NULL);
EXPECT_STR(strsub("ab123cd456xyz", "[0-9]", "#"), "ab###cd###xyz");
EXPECT_STR(strsub("ab123cd456xyz", "<1->[0-9]", "#"), "ab#cd#xyz");

EXPECT_STR(uppercase("123abcXYZ"), "123ABCXYZ");
EXPECT_STR(lowercase("123abcXYZ"), "123abcxyz");
EXPECT_STR(utf8conv("\344"), "\303\244");
EXPECT_STR(lexsortkey("12-Aaöx."), "12-aaoex.");
