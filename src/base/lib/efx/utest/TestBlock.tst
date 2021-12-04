#!/usr/bin/env esh
loadlib(".build/UnitTest.gcov.so", "UnitTest_setup");

TEST()
{
	EXPECT(true);
}

TEST (bad, 1)
{
	EXPECT(false);
}

TEST(abort, -6)
{
	PRINT("abort");
	raise(SIGABRT);
}
