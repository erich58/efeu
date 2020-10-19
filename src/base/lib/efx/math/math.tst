#!/usr/bin/env .build/script.tst

FrequencyData freq;
EXPECT_STR(freq, "{0, 0}");
EXPECT_REPR(freq, "{0., 0.}");
EXPECT_DBL(freq.n, 0.);
EXPECT_DBL(freq.x, 0.);
freq += 1.;
freq += 3.;
freq += 2.;
EXPECT_DBL(freq.n, 3.);
EXPECT_DBL(freq.x, 6.);
freq += FrequencyData(2.) * 5.;
EXPECT_DBL(freq.n, 8.);
EXPECT_DBL(freq.x, 16.);
EXPECT_DBL(freq.mean, 2.);

List_t l = freq;
EXPECT_DBL(l.obj, 8.);
EXPECT_DBL(l.next.obj, 16.);

freq = { 2., 10. };
EXPECT_DBL(freq.mean, 5.); 

freq = 2. * FrequencyData(2.) + FrequencyData(3.) * 5;
freq *= 2.;
EXPECT_DBL(freq.n, 14.);
EXPECT_DBL(freq.x, 2. * (2. * 2. + 3. * 5.));

VarianceData var;
EXPECT_STR(var, "{ 0, 0, 0 }");
EXPECT_REPR(var, "{0., 0., 0.}");
var += 2.;
EXPECT(var == { 1., 2., 4. });
var *= 2.;
EXPECT(var == { 2., 4., 8. });
EXPECT_DBL(var.n, 2.);
EXPECT_DBL(var.x, 4.);
EXPECT_DBL(var.xx, 8.);
EXPECT_DBL(var.mean, 2.);
EXPECT_DBL(var.var, 0.);
EXPECT_DBL(var.s, 0.);
freq = var;
EXPECT(freq == { 2., 4. });

var = 2. * VarianceData(2.) + VarianceData(3.) * 5;
EXPECT(var == { 7., 4. + 15., 2.*4 + 5.*9. });
var *= 2;
EXPECT(var == { 14., 2. * (4. + 15.), 2. * (2.*4 + 5.*9.) });
var = { 2., 4., 8. };
EXPECT(var == { 2., 4., 8. });

CovarianceData cov;
EXPECT_STR(cov, "{ 0, 0, 0, 0, 0, 0 }");
