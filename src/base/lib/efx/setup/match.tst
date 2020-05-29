#!/usr/bin/env .build/script.tst

EXPECT(MatchPar("*").exec("abc"));

EXPECT_REPR(/.*/, "RegExp: /.*/");
EXPECT_STR(/.*/, ".*");
EXPECT_REPR(RegExp(".*"), "RegExp: /.*/");
EXPECT_REPR(RegExp(".*", true), "RegExp: /.*/i");
EXPECT_REPR(/.*/I, "RegExp: /.*/i");

EXPECT_STR(/a\/b/, "a/b");
EXPECT_STR(/a\zb/, "a\\zb");

EXPECT(/.*/.exec("abc"));
EXPECT(/.*/.match("abc") == {"abc"});
EXPECT(/:([0-9]*):/.match("abc:123:xyz") == {":123:", "123"});
EXPECT(regmatch(/:([0-9]*):/, "abc:123:xyz") == {":123:", "123"});
EXPECT(regmatch(/:([0-9]*):/, "abcxyz") == NULL);
EXPECT(regmatch(NULL, "abcxyz") == NULL);

EXPECT((/a/ == /a/) == true);
EXPECT((/a/ == /a/i) == false);
EXPECT((/a/ == /b/) == false);
EXPECT((/a/ != /a/) == false);
EXPECT((/a/ != /a/i) == true);
EXPECT((/a/ != /b/) == true);

EXPECT(/b/ == "abc");
EXPECT("abc" == /b/);
EXPECT(/b/ != "xyz");
EXPECT("xyz" != /b/);
EXPECT("abc" != /B/);
EXPECT("abc" == /B/i);

EXPECT_STR("abcd" : /x/, NULL);
EXPECT_STR("abcd" : /bc/, "bc");
EXPECT_STR("abcd" : /b(c)/, "c");

EXPECT_STR(regsub(/bc/, "-", "abcd,abcd"), "a-d,abcd");
EXPECT_STR(regsub(/bc/, "-", "abcd,abcd", false), "a-d,abcd");
EXPECT_STR(regsub(/bc/, "", "abcd,abcd", true), "ad,ad");
EXPECT_STR(regsub(NULL, NULL, "abcd"), "abcd");
EXPECT_STR(regsub(/./, NULL, "abcd", true), NULL);
EXPECT_STR(regsub(/./, "-", "abcd", true), "----");
EXPECT_STR(regsub(/./, "-", "abcd", false), "-bcd");
EXPECT_STR(regsub(/[0-9]/, "#", "ab123cd456xyz", true), "ab###cd###xyz");
EXPECT_STR(regsub(/[0-9]+/, "#", "ab123cd456xyz", true), "ab#cd#xyz");

