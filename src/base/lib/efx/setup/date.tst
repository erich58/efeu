#!/usr/bin/env .build/script.tst

Time t = Time("2020-04-01 12:20:30");
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

Date d = Date(t);
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

/* Aktuelles Datum: Vergleich der Aufrufe */

EXPECT(Date() == today());
EXPECT(Time(time()) == Time());
EXPECT(localtime(30) - Time() == 30);
