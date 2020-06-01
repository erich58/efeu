#!/bin/sh
#if 0
export LANG=C
export TZ=GMT
exec .build/script.tst $0 arg
#endif

PRINT_LABEL("Datentyp Time");
TEST_EXPR(Time t = Time("2020-04-01 12:20:30"));
EXPECT_IOTEST(t, "Time(\"2020-04-01 12:20:30\")");
EXPECT_STR(t, "2020-04-01 12:20:30");
EXPECT_DBL(t, 2020.25425);
EXPECT(t.year == 2020);
EXPECT(t.month == 4);
EXPECT(t.day == 1);
EXPECT(t.wday == 3);
EXPECT(t.yday == 31+29+31+1);
EXPECT(t.hour == 12);
EXPECT(t.min == 20);
EXPECT(t.sec == 30);
t.print(OUT());
EXPECT_OUT("2020-04-01 12:20:30");
fprint(OUT(), t);
EXPECT_OUT("2020-04-01 12:20:30");
EXPECT_STR(str(t, "%m/%d %H:%M:%S"), "04/01 12:20:30");

{
	Time t1 = t;
	t1 += 60;
	EXPECT_STR(t1, "2020-04-01 12:21:30");
	t1 -= 22;
	EXPECT_STR(t1, "2020-04-01 12:21:08");
}

PRINT_LABEL("Datentyp time");
TEST_EXPR(time ct = 1234567);
EXPECT_IOTEST(ct, 1234567);
EXPECT_DBL(ct, 1234567.);
EXPECT_DBL((float) ct, 1234567.);
EXPECT((int)      ct == 1234567);
EXPECT((int32_t)  ct == 1234567);
EXPECT((int64_t)  ct == 1234567);
EXPECT((varint)   ct == 1234567);
EXPECT((unsigned) ct == 1234567);
EXPECT((uint32_t) ct == 1234567);
EXPECT((uint64_t) ct == 1234567);
EXPECT((varsize)  ct == 1234567);

EXPECT(time(1234567).idx == 1234567);
EXPECT(time(1234567l).idx == 1234567);
EXPECT(time(1234567u).idx == 1234567);
EXPECT(time(1234567ul).idx == 1234567);
EXPECT(time(1234567.).idx == 1234567);

TEST_EXPR(ct = t);
EXPECT(ct.year == 2020);
EXPECT(ct.month == 4);
EXPECT(ct.day == 1);
EXPECT(ct.wday == 3);
EXPECT(ct.yday == 31+29+31+1);
EXPECT(ct.hour == 12);
EXPECT(ct.min == 20);
EXPECT(ct.sec == 30);
EXPECT(ct.dst == false);
EXPECT_STR(ct, "2020-04-01 12:20:30");
fprint(OUT(), ct);
EXPECT_OUT("2020-04-01 12:20:30");

PRINT_LABEL("Datentyp Date");
TEST_EXPR(Date d = t);
EXPECT(d == Date(t));
EXPECT_IOTEST(d, "Date(01,04,2020)");
EXPECT_STR(d, "2020-04-01");
EXPECT(d == Date("2020-04-01"));
EXPECT(d.year == 2020);
EXPECT(d.month == 4);
EXPECT(d.day == 1);
EXPECT(d.wday == 3);
EXPECT(d.yday == 31+29+31+1);
int n = d.cal1900;
EXPECT_STR(Date(n), "2020-04-01");
EXPECT(Date::cal1900 == 0);

d.print(OUT());
EXPECT(d);
EXPECT(d == Date(1, 4, 2020));
EXPECT(cmp(d, Date(1, 4, 2020)) == 0);
EXPECT_OUT("2020-04-01");
fprint(OUT(), d);
EXPECT_OUT("2020-04-01");
EXPECT_STR(str(d, "%m/%d %H:%M:%S"), "04/01 12:00:00");
EXPECT_DBL(d, 2020.25);
EXPECT_STR((Time) d, "2020-04-01 12:00:00");
EXPECT_STR((time) d, "2020-04-01 12:00:00");

EXPECT(LeapYear(t));
EXPECT(LeapYear(d));
EXPECT(LeapYear(1900) == false);
EXPECT(LeapYear(2000) == true);
EXPECT(LeapYear(2020) == true);

EXPECT(jdiff(Date(1, 8, 2020), Date(1,2,2019)) == 1);
EXPECT(Date(1, 3, 2020) - Date(1,2,2020) == 29);

str def = "2020-01 Rest";
EXPECT(datecut(def, false) == Date(1, 1, 2020));
EXPECT_STR(def, " Rest");
str def = "2020-01";
EXPECT(datecut(def, true) == Date(31, 1, 2020));
EXPECT_STR(def, NULL);

timeval tv = 2.5;
EXPECT_IOTEST(tv, "2.500000");
fprint(OUT(), tv);
EXPECT_OUT("2.500000");
print(OUT(), (timeval) 2.9999994);
EXPECT_OUT("2.999999");
print(OUT(), (timeval) 2.9999996);
EXPECT_OUT("3.000000");
print(OUT(), (timeval) -0.5);
EXPECT_OUT("0.000000");

/* Aktuelles Datum: Vergleich der Aufrufe */

EXPECT_DBL(xdiv(timeval(), time()), 1.0);
EXPECT(Date() == today());
EXPECT(Time(time()) == Time());
EXPECT(localtime(30) - Time() == 30);

EXPECT(time.classification("c1h", "daytime[%h Uhr]=1h"));
EXPECT_STR(((time)Time("2020-01-01 08:20:00")).c1h, "08 Uhr");
EXPECT_STR(((time)Time("2020-01-01 15:00:00")).c1h, "15 Uhr");

EXPECT(time.classification("cday", "date[%d.%m.]=2020"));
EXPECT_STR(((time)Time("2020-05-12 08:20:00")).cday, "12.05.");
EXPECT(((time)Time("2021-01-01 15:00:00")).cday == 0);
