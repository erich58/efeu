#!/usr/bin/env .build/script.tst

extern 3;
extern int x = 5;
extern x;
EXPECT(x == 5);
extern enum ENUM { A = 1 } e = 1;
EXPECT(e == ENUM::A);
