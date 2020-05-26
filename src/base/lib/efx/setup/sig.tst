#!/usr/bin/env .build/script.tst

sleep(0.);

TEST_FORK
{
	EXPECT(sleep(0.001));
	EXPECT(sleep(1.));
	TEST_EXIT;
}

sleep(0.1);
kill(UnitTest::child, SIGUSR1);
CHILD_SIGNALED(SIGUSR1);

TEST_FORK
{
	EXPECT(sleep(1.));
	TEST_EXIT;
}

sleep(0.1);
kill(UnitTest::child, SIGTERM);
CHILD_EXIT(1);

TEST_FORK
{
	EXPECT(signal(SIGTERM, SIG_DFL));
	EXPECT(sleep(1.));
	TEST_EXIT;
}

sleep(0.1);
kill(UnitTest::child, SIGTERM);
CHILD_SIGNALED(SIGTERM);

TEST_FORK
{
	EXPECT(signal(SIGTERM, SIG_IGN));
	EXPECT(sleep(0.2));
	TEST_EXIT;
}

sleep(0.1);
kill(UnitTest::child, SIGTERM);
CHILD_EXIT(0);

TEST_FORK
{
	EXPECT(signal(SIGTERM, [ PRINT("catched"); ]));
	EXPECT(sleep(0.2) == false);
	TEST_EXIT;
}

sleep(0.1);
kill(UnitTest::child, SIGTERM);
CHILD_EXIT(0);

TEST_FORK
{
	EXPECT(signal(SIGTERM, [ PRINT("catched"); ]));
	EXPECT(sleep(0.2, true));
	TEST_EXIT;
}

sleep(0.1);
kill(UnitTest::child, SIGTERM);
CHILD_EXIT(0);

TEST_FORK
{
	signal(SIGALRM, [ EXPECT(this == SIGALRM); ]);
	alarm(1);
	sleep(2.);
	TEST_EXIT;
}

CHILD_EXIT(0);

TEST_FORK
{
	signal(SIGUSR1, [ EXPECT(this == SIGUSR1); ]);
	raise(SIGUSR1);
	sleep(2.);
	TEST_EXIT;
}

CHILD_EXIT(0);
