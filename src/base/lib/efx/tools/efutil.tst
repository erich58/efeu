#!/usr/bin/env .build/script.tst

enum X { a, b };

EXPECT(cmp(X::a, X::b) == -1);
EXPECT(cmp(X::a, X::a) == 0);
EXPECT(cmp(X::b, X::a) == 1);
EXPECT(rcmp(X::a, X::b) == 1);
EXPECT(rcmp(X::a, X::a) == 0);
EXPECT(rcmp(X::b, X::a) == -1);

EXPECT(cmp(1., 2.) == -1);
EXPECT(cmp(1., 1.) == 0);
EXPECT(cmp(2., 1.) == 1);
EXPECT(rcmp(1., 2.) == 1);
EXPECT(rcmp(1., 1.) == 0);
EXPECT(rcmp(2., 1.) == -1);

// Test in efmain
EXPECT(cmp("a", "a") == 0);
EXPECT(rcmp("a", "a") == 0);
EXPECT(lexcmp("a3B", "A3b") == 0);
EXPECT(alnumcmp("a3.c", "a3:c") == 0);
EXPECT(alphacmp("a3.c", "a5:c") == 0);

UnitTest::stdin("stdin", "data");
UnitTest::stdout("stdout");
EXPECT_STR(inquire("Prompt:"), "data");
UnitTest::stdin();
UnitTest::stdout();
TEST_FILE("stdout", "Prompt:");

UnitTest::stdout("stdout");
EXPECT(TeXquote("abc") == 7);
EXPECT(TeXput('-') == 3);
EXPECT(TeXput("|") == 3);
UnitTest::stdout();
TEST_FILE("stdout", "\"\`abc\"'{-}$|$");
