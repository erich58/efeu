#!/usr/bin/env .build/script.tst

EXPECT(abs(-2) == 2);
EXPECT(abs(-32u) == -32u);
EXPECT(min(3, 5) == 3);
EXPECT(max(3, 5) == 5);
EXPECT(cmp(2, 5) == -1);
EXPECT(cmp(2, 2) == 0);
EXPECT(rcmp(2, 5) == 1);
EXPECT(rcmp(5, 5) == 0);

EXPECT((float) 3 == 3.);
EXPECT((double) 3 == 3.);

EXPECT((bool) 5 == 1);
EXPECT(!5 == 0);
EXPECT(~0 == -1);
EXPECT(-(-5) == 5);
EXPECT(+(-5) == -5);
EXPECT((int) 3.5 == 3);

EXPECT(3 * 5 == 15);
EXPECT(5 / 2 == 2);
EXPECT(5 % 2 == 1);
EXPECT(2 + 5 == 7);
EXPECT(5 - 7 == -2);

EXPECT(1 << 2 == 4);
EXPECT(64 >> 2 == 16);
EXPECT((0xF0 & 0x1F) == 0x10);
EXPECT((0b1100 ^ 0b0110) == 0b1010);
EXPECT((0xF0 | 0x03) == 0xF3);

EXPECT(2 < 5);
EXPECT(2 <= 5);
EXPECT(2 == 2);
EXPECT(2 != 5);
EXPECT(5 > 2);
EXPECT(5 >= 2);

int x = 5;
EXPECT(x++ == 5);
EXPECT(++x == 7);
EXPECT(--x == 6);
EXPECT(x-- == 6);
EXPECT((x *= 2) == 10);
EXPECT((x /= 3) == 3);
EXPECT((x %= 2) == 1);
EXPECT((x += 6) == 7);
EXPECT((x -= 2) == 5);
EXPECT((x |= 2) == 7);
EXPECT((x &= 12) == 4);
EXPECT((x ^= 2) == 6);
EXPECT((x <<= 1) == 12);
EXPECT((x >>= 2) == 3);
