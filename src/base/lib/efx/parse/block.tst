#!/usr/bin/env .build/script.tst

str x = "";

for (int n = 0; n < 5; ++n)
{
	if	(n == 2) continue;
	if	(n == 4) break;

	x += sprintf("%d", n);
}

EXPECT_STR(x, "013");

x = "";

for (n in 0:5)
{
	if	(n == 2) continue;
	if	(n == 4) break;

	x += sprintf("%d", n);
}

EXPECT_STR(x, "013");

str f1()
{
	for (int n = 0; n < 5; ++n)
		if	(n == 3)	return 1;

	return 0;
}

EXPECT(f1() == 1);

str f2()
{
	for (n in 0:5)
		if	(n == 3)	return 1;

	return 0;
}

EXPECT(f2() == 1);

TEST_FORK
{
	for (break;;)
		exit(1);

	exit(0);
}

CHILD_EXIT(0);
