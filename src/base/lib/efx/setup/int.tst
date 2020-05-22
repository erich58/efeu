#!/usr/bin/env .build/script.tst

PRINT_LABEL("int");
EXPECT_STR(typeof 0, "int");
EXPECT((int)    -128 == -128);
EXPECT((int8_t) -128 == -128);
EXPECT((int16_t)-128 == -128);
EXPECT((int32_t)-128 == -128);
EXPECT((int64_t)-128 == -128);
EXPECT((unsigned)0x7FFF == 0x7FFF);
EXPECT((uint8_t) 0x7FFF == 0xFF);
EXPECT((uint16_t)0x7FFF == 0x7FFF);
EXPECT((uint32_t)0x7FFF == 0x7FFF);
EXPECT((uint64_t)0x7FFF == 0x7FFF);
EXPECT_STR(-12345, "-12345");
EXPECT_REPR(-12345, "-12345");

PRINT_LABEL("unsigned");
EXPECT_STR(typeof ~0u, "unsigned");
EXPECT((int)    ~0u == -1);
EXPECT((int8_t) ~0u == -1);
EXPECT((int16_t)~0u == -1);
EXPECT((int32_t)~0u == -1);
EXPECT((int64_t)~0u == 0xFFFFFFFFll);
EXPECT((unsigned)~0u == 0xFFFFFFFFull);
EXPECT((uint8_t) ~0u == 0xFF);
EXPECT((uint16_t)~0u == 0xFFFF);
EXPECT((uint32_t)~0u == 0xFFFFFFFFull);
EXPECT((uint64_t)~0u == 0xFFFFFFFFull);
EXPECT_STR(12345u, "12345");

PRINT_LABEL("int8_t")
EXPECT((int)    (int8_t)0x80 == -128);
EXPECT((int8_t) (int8_t)0x80 == -128);
EXPECT((int16_t)(int8_t)0x80 == -128);
EXPECT((int32_t)(int8_t)0x80 == -128);
EXPECT((int64_t)(int8_t)0x80 == -128);
EXPECT((unsigned)(int8_t)0x7f == 127);
EXPECT((uint8_t) (int8_t)0x7f == 127);
EXPECT((uint16_t)(int8_t)0x7f == 127);
EXPECT((uint32_t)(int8_t)0x7f == 127);
EXPECT((uint64_t)(int8_t)0x7f == 127);

PRINT_LABEL("uint8_t");
EXPECT((int)    (uint8_t)~0 == 0xFF);
EXPECT((int8_t) (uint8_t)~0 == -1);
EXPECT((int16_t)(uint8_t)~0 == 0xFF);
EXPECT((int32_t)(uint8_t)~0 == 0xFF);
EXPECT((int64_t)(uint8_t)~0 == 0xFF);
EXPECT((unsigned)(uint8_t)~0 == 0xFF);
EXPECT((uint8_t) (uint8_t)~0 == 0xFF);
EXPECT((uint16_t)(uint8_t)~0 == 0xFF);
EXPECT((uint32_t)(uint8_t)~0 == 0xFF);
EXPECT((uint64_t)(uint8_t)~0 == 0xFF);

PRINT_LABEL("int16_t");
EXPECT((int)    (int16_t)-128 == -128);
EXPECT((int8_t) (int16_t)-128 == -128);
EXPECT((int16_t)(int16_t)-128 == -128);
EXPECT((int32_t)(int16_t)-128 == -128);
EXPECT((int64_t)(int16_t)-128 == -128);
EXPECT((unsigned)(int16_t)0x7FFF == 0x7FFF);
EXPECT((uint8_t) (int16_t)0x7FFF == 0xFF);
EXPECT((uint16_t)(int16_t)0x7FFF == 0x7FFF);
EXPECT((uint32_t)(int16_t)0x7FFF == 0x7FFF);
EXPECT((uint64_t)(int16_t)0x7FFF == 0x7FFF);

PRINT_LABEL("uint16_t");
EXPECT((int)    (uint16_t)~0 == 0xFFFF);
EXPECT((int8_t) (uint16_t)~0 == -1);
EXPECT((int16_t)(uint16_t)~0 == -1);
EXPECT((int32_t)(uint16_t)~0 == 0xFFFF);
EXPECT((int64_t)(uint16_t)~0 == 0xFFFF);
EXPECT((unsigned)(uint16_t)~0 == 0xFFFF);
EXPECT((uint8_t) (uint16_t)~0 == 0xFF);
EXPECT((uint16_t)(uint16_t)~0 == 0xFFFF);
EXPECT((uint32_t)(uint16_t)~0 == 0xFFFF);
EXPECT((uint64_t)(uint16_t)~0 == 0xFFFF);

PRINT_LABEL("int32_t");
EXPECT((int)    (int32_t)-128 == -128);
EXPECT((int8_t) (int32_t)-128 == -128);
EXPECT((int16_t)(int32_t)-128 == -128);
EXPECT((int32_t)(int32_t)-128 == -128);
EXPECT((int64_t)(int32_t)-128 == -128);
EXPECT((unsigned)(int32_t)0x7FFF == 0x7FFF);
EXPECT((uint8_t) (int32_t)0x7FFF == 0xFF);
EXPECT((uint16_t)(int32_t)0x7FFF == 0x7FFF);
EXPECT((uint32_t)(int32_t)0x7FFF == 0x7FFF);
EXPECT((uint64_t)(int32_t)0x7FFF == 0x7FFF);
EXPECT_STR((int32_t)-12345, "-12345");
EXPECT_REPR((int32_t)-12345, "-12345");

PRINT_LABEL("uint32_t");
EXPECT((int)    (uint32_t)~0 == -1);
EXPECT((int8_t) (uint32_t)~0 == -1);
EXPECT((int16_t)(uint32_t)~0 == -1);
EXPECT((int32_t)(uint32_t)~0 == -1);
EXPECT((int64_t)(uint32_t)~0 == 0xFFFFFFFFll);
EXPECT((unsigned)(uint32_t)~0 == 0xFFFFFFFFull);
EXPECT((uint8_t) (uint32_t)~0 == 0xFF);
EXPECT((uint16_t)(uint32_t)~0 == 0xFFFF);
EXPECT((uint32_t)(uint32_t)~0 == 0xFFFFFFFFull);
EXPECT((uint64_t)(uint32_t)~0 == 0xFFFFFFFFull);
EXPECT_STR((uint32_t) 12345, "12345");
EXPECT_REPR((uint32_t) 12345, "12345");

PRINT_LABEL("int64_t");
EXPECT((int)    (int64_t)-128 == -128);
EXPECT((int8_t) (int64_t)-128 == -128);
EXPECT((int16_t)(int64_t)-128 == -128);
EXPECT((int32_t)(int64_t)-128 == -128);
EXPECT((int64_t)(int64_t)-128 == -128);
EXPECT((unsigned)(int64_t)0x7FFF == 0x7FFF);
EXPECT((uint8_t) (int64_t)0x7FFF == 0xFF);
EXPECT((uint16_t)(int64_t)0x7FFF == 0x7FFF);
EXPECT((uint32_t)(int64_t)0x7FFF == 0x7FFF);
EXPECT((uint64_t)(int64_t)0x7FFF == 0x7FFF);
EXPECT_STR((int64_t) -1234567890, "-1234567890");
EXPECT_REPR((int64_t) -1234567890, "-1234567890");

PRINT_LABEL("uint64_t");
EXPECT((int)    (uint64_t)~0 == -1);
EXPECT((int8_t) (uint64_t)~0 == -1);
EXPECT((int16_t)(uint64_t)~0 == -1);
EXPECT((int32_t)(uint64_t)~0 == -1);
EXPECT((int64_t)(uint64_t)~0 == -1);
EXPECT((unsigned)(uint64_t)~0 == 0xFFFFFFFFull);
EXPECT((uint8_t) (uint64_t)~0 == 0xFF);
EXPECT((uint16_t)(uint64_t)~0 == 0xFFFF);
EXPECT((uint32_t)(uint64_t)~0 == 0xFFFFFFFFull);
EXPECT((uint64_t)(uint64_t)~0 == 0xFFFFFFFFFFFFFFFFull);
EXPECT_STR((uint64_t) 1234567890, "1234567890");
EXPECT_REPR((uint64_t) 1234567890, "1234567890");

PRINT_LABEL("varint");
EXPECT_STR(varint.cname, "int64_t");
EXPECT_STR(typeof 0l, "varint");
EXPECT((int)    -128l == -128);
EXPECT((int8_t) -128l == -128);
EXPECT((int16_t)-128l == -128);
EXPECT((int32_t)-128l == -128);
EXPECT((int64_t)-128l == -128);
EXPECT((unsigned)0x7FFFl == 0x7FFF);
EXPECT((uint8_t) 0x7FFFl == 0xFF);
EXPECT((uint16_t)0x7FFFl == 0x7FFF);
EXPECT((uint32_t)0x7FFFl == 0x7FFF);
EXPECT((uint64_t)0x7FFFl == 0x7FFF);
EXPECT_STR(-1234567890l, "-1234567890");

PRINT_LABEL("varsize");
EXPECT_STR(varsize.cname, "uint64_t");
EXPECT_STR(typeof 0ul, "varsize");
EXPECT((int)    ~0ul == -1);
EXPECT((int8_t) ~0ul == -1);
EXPECT((int16_t)~0ul == -1);
EXPECT((int32_t)~0ul == -1);
EXPECT((int64_t)~0ul == -1);
EXPECT((unsigned)~0ul == 0xFFFFFFFFull);
EXPECT((uint8_t) ~0ul == 0xFF);
EXPECT((uint16_t)~0ul == 0xFFFF);
EXPECT((uint32_t)~0ul == 0xFFFFFFFFull);
EXPECT((uint64_t)~0ul == 0xFFFFFFFFFFFFFFFFull);
EXPECT_STR(1234567890ul, "1234567890");
EXPECT_REPR(1234567890ul, "1234567890");

PRINT_LABEL("Integer functions");
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

{
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
}
