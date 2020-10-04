#!/usr/bin/env .build/script.tst

EXPECT(fprintf(OUT(), NULL) == 0);
EXPECT(fprintf(OUT(), "%d", 42) == 2);
EXPECT_OUT(42);
EXPECT_STR(sprintf("%d", 42), "42");

EXPECT_STR(sprintf("%d"), "0");
EXPECT_STR(sprintf("|%-5d|%%", 1), "|1    |%");
EXPECT_STR(sprintf("|%*.*s|", 5, 3, "123456"), "|  123|");
EXPECT_STR(sprintf("|%*.*s|", -5, -2, "123456"), "|56   |");

EXPECT_STR(sprintf("%c", 'a'), "a");
EXPECT_STR(sprintf("%lc", 'a'), "a");

EXPECT_STR(sprintf("%hhX", ~0), "FF");
EXPECT_STR(sprintf("%hX", ~0), "FFFF");
EXPECT_STR(sprintf("%lX", ~0), "FFFFFFFF");
EXPECT_STR(sprintf("%llX", ~0), "FFFFFFFFFFFFFFFF");
EXPECT_STR(sprintf("%jd", 0), "0");
EXPECT_STR(sprintf("%zd", 0), "0");
EXPECT_STR(sprintf("%td", 0), "0");

EXPECT_STR(sprintf("%.2f", 3.5), "3.50");
MATCH_STR(sprintf("%p", NULL), "^0x");

int x;
EXPECT_STR(sprintf("äöü%nxyz", x), "äöüxyz");
EXPECT(x == 3);
EXPECT_STR(sprintf("äöü%#nxyz", x), "äöüxyz");
EXPECT(x == 6);

EXPECT_STR(sprintf("|%5.2[abc]|", x), "|   ab|");
