#!/usr/bin/env .build/script.tst

EXPECT_DBL(sqrt(25.), 5.);
EXPECT_DBL(log(1.), 0.);
EXPECT_DBL(xlog(-2.), 0.);
EXPECT_DBL(exp(0.), 1.);
EXPECT_DBL(ceil(2.1), 3.);
EXPECT_DBL(floor(-3.2), -4.);
EXPECT_DBL(sin(0.), 0.);
EXPECT_DBL(cos(0.), 1.);
EXPECT_DBL(tan(0.), 0.);
EXPECT_DBL(asin(0.), 0.);
EXPECT_DBL(acos(1.), 0.);
EXPECT_DBL(atan(1.), 0.25*M_PI);
EXPECT_DBL(atan2(-1.0, -1.0), -0.75*M_PI);
EXPECT_DBL(pow(2.0, 3.), 8.);
EXPECT_DBL(3.8 % 3.5, 0.3);
EXPECT_DBL(rnd125(0.), 0.);
EXPECT_DBL(rnd125(-0.21), -0.5);
EXPECT_DBL(rnd125(7.8), 10.);
EXPECT_DBL(ldexp(10., 4), 160.);

{
	int n = 0;
	double x = 0.;
	EXPECT_DBL(modf(3.75, x), 0.75);
	EXPECT_DBL(x, 3.0);
	EXPECT_DBL(frexp(2.5, n), 2.5 / 4.);
	EXPECT(n == 2);
}
