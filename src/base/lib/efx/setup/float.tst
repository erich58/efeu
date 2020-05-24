#!/usr/bin/env .build/script.tst

EXPECT_IOTEST(float(3.5), "3.5000000");
EXPECT_IOTEST(3.5, "3.500000000000000");
EXPECT_STR(3.5, "3.5000");

fprint(OUT(), 3.5);
EXPECT_OUT("3.5000");

EXPECT(abs(-2.) == 2.);
EXPECT(min(3., 5.) == 3.);
EXPECT(max(3., 5.) == 5.);

EXPECT(5.);
EXPECT(!0.);
EXPECT(-(-5.) == 5.);
EXPECT(+(-5.) == -5.);

EXPECT(3. * 5. == 15.);
EXPECT(5. / 2. == 2.5);
EXPECT(2. + 5.5 == 7.5);
EXPECT(5.25 - 7. == -1.75);

EXPECT(2.25 < 2.5);
EXPECT(2.25 <= 2.5);
EXPECT(2.5 == 2.5);
EXPECT(2.25 != 2.5);
EXPECT(2.5 > 2.25);
EXPECT(2.5 >= 2.25);

{
	double x = 5.5;
	EXPECT_DBL(x++, 5.5);
	EXPECT_DBL(++x, 7.5);
	EXPECT_DBL(--x, 6.5);
	EXPECT_DBL(x--, 6.5);
	EXPECT_DBL(x *= 2., 11.);
	EXPECT_DBL(x /= 4., 2.75);
	EXPECT_DBL(x += 6., 8.75);
	EXPECT_DBL(x -= 2.5, 6.25);
}

EXPECT(xdiv(5., 0.) == 0.);
EXPECT(xdiv(5., 2.) == 2.5);
EXPECT(xdiv1(5., 0.) == 1.);
EXPECT(xdiv1(5., 2.) == 2.5);
EXPECT(idx(5., 20.) == 25.);
EXPECT(idx(5., 0.) == 0.);
EXPECT(rate(8., 10.) == -20.);
EXPECT(rate(12., 10.) == 20.);
EXPECT(rate(10., 0.) == 0.);
EXPECT(rnd(2.4) == 2.);
EXPECT(rnd(2.5) == 3.);
EXPECT(rnd(-2.5) == -3.);

EXPECT_DBL(M_PI, 3.14159265);
EXPECT(1./0. == Inf);
EXPECT_STR(str(0./0.), "NaN");
EXPECT_STR(NaN, "NaN");
