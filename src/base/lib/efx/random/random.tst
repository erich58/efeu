#!/usr/bin/env .build/script.tst

EXPECT_REPR(Random, "Random");

UnitTest::stdout("output");
Info("print:RNG");
UnitTest::stdout(NULL);
MATCH_CONTENT("output", string !
random number generators
d48	.*
std	.*
!);

UnitTest::stderr("error");
EXPECT_REPR(Random("undef", 0), ":std:0X000001");
UnitTest::stderr(NULL);
MATCH_CONTENT("error", "undef");

EXPECT_REPR(Random("std", 0xF0), ":std:0X0000F0");
EXPECT_REPR(Random("d48", 0xF0), ":d48:0X000000F0330E");

Random r = NULL;
EXPECT_REPR(r, NULL);
Random r = Random(0);
EXPECT_REPR(r, ":std:0X000001");
EXPECT(r.rand() == 0X41C67EA6);
EXPECT_REPR(r, ":std:0X41C67EA6");
EXPECT_REPR(r.copy(), ":std:0X41C67EA6");
r.seed(0x1234);
EXPECT_REPR(r, ":std:0X001234");

Random r = Random(":d48");
EXPECT_REPR(r, ":d48:0X00000000330E");
EXPECT(r.rand() == 0X15DDB16E);
EXPECT_REPR(r, ":d48:0X2BBB62DC5101");
EXPECT_REPR(r.copy(), ":d48:0X2BBB62DC5101");

// Aufrufe wegen Coverage 
r.uniform();
r.normal();
r.linear(0.5);
r.linear(10.);
r.linear(-10.);
r.poisson(10.);
r.poisson(100.);
r.round(2.5);
r.round(-2.5);
r.index(10);
srand();
drand();

int x[] = { 1, 2, 3, 4, 5, 6 };
r.seed(0);
EXPECT(r.choice(x, 3) == 3);
EXPECT_STR(x, "{2, 5, 3, 4, 1, 6}");
