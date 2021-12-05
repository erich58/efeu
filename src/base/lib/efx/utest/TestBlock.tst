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

/* TODO */

TEST_FORK
{
	TEST ()
	{
		EXPECT(false);
	};	// Warum ';' notwendig ?
}

/* Einrückung der Ausgabe nicht OK */
CHILD_EXIT(0); // Warum exit 0 anstelle von 1 ?
