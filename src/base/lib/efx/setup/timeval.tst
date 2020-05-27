#!/usr/bin/env .build/script.tst

timeval t = 2.5;
EXPECT_IOTEST(t, "2.500000");
EXPECT_DBL(xdiv(timeval(), time()), 1.0);
fprint(OUT(), t);
EXPECT_OUT("2.500000");
print(OUT(), (timeval) 2.9999994);
EXPECT_OUT("2.999999");
print(OUT(), (timeval) 2.9999996);
EXPECT_OUT("3.000000");
print(OUT(), (timeval) -0.5);
EXPECT_OUT("0.000000");
