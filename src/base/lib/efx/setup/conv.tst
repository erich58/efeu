#!/usr/bin/env .build/script.tst

EXPECT((bool) NULL == false);
EXPECT((Type_t) "int" == int);
EXPECT(List_t(1) == { 1 });
EXPECT((int) true == 1);
EXPECT((unsigned) true == 1);
EXPECT((uint64_t) true == 1);
EXPECT((double) true == 1.);
EXPECT(bool(0.) == false);
EXPECT(bool(0.001) == true);
EXPECT((float) 1. == 1.);

{
	int x[] = 0 : 5;
	EXPECT((List_t) x = { 0, 1, 2, 3, 4, 5 });
	EXPECT(List_t(x, 2) = { 2, 3, 4, 5 });
	EXPECT(List_t(x, 1, 3) = { 1, 2, 3 });
}

{
	struct S { int a; int b } s = { 1, 2 };
	EXPECT((List_t) (VarTab) S == { "a", "b" });
}
