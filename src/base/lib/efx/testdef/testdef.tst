#!/usr/bin/env .build/script.tst

TEST(num)
{
	TestDef t = TestNum("");
	EXPECT(t(0) == true);
	MATCH_REPR(t, "TestDef: TestParDef"); 

	TestDef t = TestNum("-1");
	EXPECT(t(0) == true);
	EXPECT(t(1) == false);
	EXPECT(t(2) == false);

	TestDef t = TestNum(":1");
	EXPECT(t(0) == true);
	EXPECT(t(1) == true);
	EXPECT(t(2) == false);

	TestDef t = TestNum("1:");
	EXPECT(t(0) == false);
	EXPECT(t(1) == true);
	EXPECT(t(2) == true);

	TestDef t = TestNum("1");
	EXPECT(t(0) == false);
	EXPECT(t(1) == true);
	EXPECT(t(2) == false);

	TestDef t = TestNum("!1");
	EXPECT(t(0) == true);
	EXPECT(t(1) == false);
	EXPECT(t(2) == true);

	TestDef t = TestNum("+1");
	EXPECT(t(1) == false);
	EXPECT(t(2) == true);

	TestDef t = TestNum("1:2");
	EXPECT(t(0) == false);
	EXPECT(t(1) == true);
	EXPECT(t(2) == true);
	EXPECT(t(3) == false);

	TestDef t = TestNum("!1:2");
	EXPECT(t(0) == true);
	EXPECT(t(1) == false);
	EXPECT(t(2) == false);
	EXPECT(t(3) == true);
}

TEST(v64)
{
	TestDef t = Test64("");
	EXPECT(t(0) == true);

	TestDef t = Test64("-1");
	EXPECT(t(0) == true);
	EXPECT(t(1) == false);
	EXPECT(t(2) == false);

	TestDef t = Test64(":1");
	EXPECT(t(0) == true);
	EXPECT(t(1) == true);
	EXPECT(t(2) == false);

	TestDef t = Test64("1:");
	EXPECT(t(0) == false);
	EXPECT(t(1) == true);
	EXPECT(t(2) == true);

	TestDef t = Test64("1");
	EXPECT(t(0) == false);
	EXPECT(t(1) == true);
	EXPECT(t(2) == false);

	TestDef t = Test64("!1");
	EXPECT(t(0) == true);
	EXPECT(t(1) == false);
	EXPECT(t(2) == true);

	TestDef t = Test64("+1");
	EXPECT(t(1) == false);
	EXPECT(t(2) == true);

	TestDef t = Test64("1:2");
	EXPECT(t(0) == false);
	EXPECT(t(1) == true);
	EXPECT(t(2) == true);
	EXPECT(t(3) == false);

	TestDef t = Test64("!1:2");
	EXPECT(t(0) == true);
	EXPECT(t(1) == false);
	EXPECT(t(2) == false);
	EXPECT(t(3) == true);
}

TEST(size)
{
	TestDef t = TestNum("0xa12345678");
	EXPECT(t(0x12345678) == true);
	EXPECT(t(0xa12345678) == true);
	EXPECT(t(0xb12345678) == true);
	TestDef t = Test64("0xa12345678");
	EXPECT(t(0x12345678) == false);
	EXPECT(t(0xa12345678) == true);
	EXPECT(t(0xb12345678) == false);
}

TEST(date)
{
	TestDef t = TestDat("2000-01-01");
	EXPECT(t((int)Date("2000-01-01")) == true);
	EXPECT(t(Date("2000-01-01").cal1900) == false);

	TestDef t = TestDat1900("2000-01-01");
	EXPECT(t(Date("2000-01-01").cal1900) == true);
	EXPECT(t((int)Date("2000-01-01")) == false);
}

TEST(code)
{
	(void) enum X { a, b, c, d, e };
	TestDef t = TestEnum("X", "b,c");
	EXPECT(t(X::a) == false);
	EXPECT(t(X::b) == true);
	EXPECT(t(2) == true);
	EXPECT(t(3) == false);

	TestDef t = TestEnum("X", "!b");
	EXPECT(t(X::a) == true);
	EXPECT(t(X::b) == false);

	TestDef t = TestEnum("X", "^b");
	EXPECT(t(X::a) == true);
	EXPECT(t(X::b) == false);

	TestDef t = TestEnum("X", "=b");
	EXPECT(t(X::a) == false);
	EXPECT(t(X::b) == true);

	TestDef t = TestEnum("X", NULL);
}

TEST(double)
{
	TestDef t = TestDouble("");
	EXPECT(t(0.5) == true);

	TestDef t = TestDouble("<1.");
	EXPECT(t(-5.) == true);
	EXPECT(t(0.99) == true);
	EXPECT(t(1.0) == false);

	TestDef t = TestDouble("!<1.");
	EXPECT(t(-5.) != true);
	EXPECT(t(0.99) != true);
	EXPECT(t(1.0) != false);

	TestDef t = TestDouble(">1.");
	EXPECT(t(1.0) == false);
	EXPECT(t(1.1) == true);

	TestDef t = TestDouble("^>1.");
	EXPECT(t(1.0) != false);
	EXPECT(t(1.1) != true);

	TestDef t = TestDouble("1.:2.");
	EXPECT(t(0.9) == false);
	EXPECT(t(1.0) == true);
	EXPECT(t(2.0) == true);
	EXPECT(t(2.1) == false);

	TestDef t = TestDouble("!1.:2.");
	EXPECT(t(0.9) != false);
	EXPECT(t(1.0) != true);
	EXPECT(t(2.0) != true);
	EXPECT(t(2.1) != false);

	TestDef t = TestDouble("1.:");
	EXPECT(t(0.9) == false);
	EXPECT(t(1.0) == true);
	EXPECT(t(99999.0) == true);

	TestDef t = TestDouble(":1.");
	EXPECT(t(-99999.0) == true);
	EXPECT(t(1.0) == true);
	EXPECT(t(1.1) == false);

	TestDef t = TestDouble("1.");
	EXPECT(t(0.9) == false);
	EXPECT(t(1.0) == true);
	EXPECT(t(1.1) == false);

	TestDef t = TestDouble("!1.");
	EXPECT(t(0.9) != false);
	EXPECT(t(1.0) != true);
	EXPECT(t(1.1) != false);
}

TEST(type)
{
	TestDef t = TestDef(int);
	EXPECT_REPR(t, "TestDef: ");
	EXPECT(t(0) == true);
}

TEST(struct)
{
	enum ENUM { a, b, c };
	struct DATA {
		int x;
		struct { ENUM z } y;
	};

	TestDef t = TestDef(DATA, ".x=5,6 y.z=b");
	MATCH_REPR(t, "TestDef: DATA");
	EXPECT(t({3, {ENUM::b}}) == false);
	EXPECT(t({5, {ENUM::b}}) == true);
	EXPECT(t({5, {ENUM::c}}) == false);
	EXPECT(t({6, {ENUM::b}}) == true);
	EXPECT(t({7, {ENUM::b}}) == false);

	TestDef t = TestDef(DATA, "xx=5");
	EXPECT(t({42, {ENUM::a}}) == true);
}

TEST(date)
{
	TestDef t = TestDef(Date, "day=5 month=8");
	MATCH_REPR(t, "TestDef: Date")
	EXPECT(t(Date(10, 8, 2020)) == false);
	EXPECT(t(Date(5, 8, 2020)) == true);
	EXPECT(t(Date(5, 10, 2020)) == false);
}

/*
enum E { e0, e1 };

TestDef te = TestDef(E);
EXPECT(te(1) == true);
UnitTest::stderr("stderr");
EXPECT(te(2.5) == false);
UnitTest::stderr();
*/
