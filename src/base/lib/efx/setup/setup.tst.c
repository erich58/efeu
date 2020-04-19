#include <EFEU/UnitTest.h>
#include <EFEU/object.h>
#include "utest/mstr.tst.h"
#include "utest/obj.tst.h"
#include <math.h>

TEST_DECL(base)
{
	EfiObj *obj;

	TEST_OBJ_TYPE(bool2Obj(0), "false");
	TEST_OBJ_TYPE(bool2Obj(1), "true");
	TEST_OBJ_TYPE(char2Obj('a'), "'a'");
	TEST_OBJ_TYPE(ucs2Obj(L'€'), "'€'");
	TEST_OBJ_TYPE(str2Obj(mstrcpy("abc")), "\"abc\"");
	TEST_OBJ_TYPE(str2Obj(NULL), "NULL");
	TEST_OBJ_TYPE(NewObj(&Type_ptr, NULL), "NULL");
}

TEST_DECL(stdint)
{
	TEST_OBJ_TYPE(int2Obj(-12345), "-12345");
	EXPECT_UTERM("(unsigned) 12345", 12345);
	EXPECT_MSTR(EVAL("-12345"), "-12345");

	TEST_OBJ_TYPE(uint2Obj(12345), "12345u");
	EXPECT_ITERM("(int)    ~0u", -1);
	EXPECT_ITERM("(int8_t) ~0u", -1);
	EXPECT_ITERM("(int16_t)~0u", -1);
	EXPECT_ITERM("(int32_t)~0u", -1);
	EXPECT_ITERM("(int64_t)~0u", 0xFFFFFFFFll);
	EXPECT_UTERM("(unsigned)~0u", 0xFFFFFFFFull);
	EXPECT_UTERM("(uint8_t) ~0u", 0xFF);
	EXPECT_UTERM("(uint16_t)~0u", 0xFFFF);
	EXPECT_UTERM("(uint32_t)~0u", 0xFFFFFFFFull);
	EXPECT_UTERM("(uint64_t)~0u", 0xFFFFFFFFull);
	EXPECT_MSTR(EVAL("12345u"), "12345");

	TEST_TERM_TYPE("(int8_t)0x80", "-128");
	EXPECT_ITERM("(int)    (int8_t)0x80", -128);
	EXPECT_ITERM("(int8_t) (int8_t)0x80", -128);
	EXPECT_ITERM("(int16_t)(int8_t)0x80", -128);
	EXPECT_ITERM("(int32_t)(int8_t)0x80", -128);
	EXPECT_ITERM("(int64_t)(int8_t)0x80", -128);
	EXPECT_ITERM("(unsigned)(int8_t)0x7f", 127);
	EXPECT_ITERM("(uint8_t) (int8_t)0x7f", 127);
	EXPECT_ITERM("(uint16_t)(int8_t)0x7f", 127);
	EXPECT_ITERM("(uint32_t)(int8_t)0x7f", 127);
	EXPECT_ITERM("(uint64_t)(int8_t)0x7f", 127);

	TEST_TERM_TYPE("(uint8_t) ~0", "255");
	EXPECT_ITERM("(int)    (uint8_t)~0", 0xFF);
	EXPECT_ITERM("(int8_t) (uint8_t)~0", -1);
	EXPECT_ITERM("(int16_t)(uint8_t)~0", 0xFF);
	EXPECT_ITERM("(int32_t)(uint8_t)~0", 0xFF);
	EXPECT_ITERM("(int64_t)(uint8_t)~0", 0xFF);
	EXPECT_UTERM("(unsigned)(uint8_t)~0", 0xFF);
	EXPECT_UTERM("(uint8_t) (uint8_t)~0", 0xFF);
	EXPECT_UTERM("(uint16_t)(uint8_t)~0", 0xFF);
	EXPECT_UTERM("(uint32_t)(uint8_t)~0", 0xFF);
	EXPECT_UTERM("(uint64_t)(uint8_t)~0", 0xFF);

	TEST_TERM_TYPE("(int16_t) -128", "-128");
	EXPECT_ITERM("(int)    (int16_t)-128", -128);
	EXPECT_ITERM("(int8_t) (int16_t)-128", -128);
	EXPECT_ITERM("(int16_t)(int16_t)-128", -128);
	EXPECT_ITERM("(int32_t)(int16_t)-128", -128);
	EXPECT_ITERM("(int64_t)(int16_t)-128", -128);
	EXPECT_UTERM("(unsigned)(int16_t)0x7FFF", 0x7FFF);
	EXPECT_UTERM("(uint8_t) (int16_t)0x7FFF", 0xFF);
	EXPECT_UTERM("(uint16_t)(int16_t)0x7FFF", 0x7FFF);
	EXPECT_UTERM("(uint32_t)(int16_t)0x7FFF", 0x7FFF);
	EXPECT_UTERM("(uint64_t)(int16_t)0x7FFF", 0x7FFF);
	
	TEST_TERM_TYPE("(uint16_t) 12345", "12345");
	EXPECT_ITERM("(int)    (uint16_t)~0", 0xFFFF);
	EXPECT_ITERM("(int8_t) (uint16_t)~0", -1);
	EXPECT_ITERM("(int16_t)(uint16_t)~0", -1);
	EXPECT_ITERM("(int32_t)(uint16_t)~0", 0xFFFF);
	EXPECT_ITERM("(int64_t)(uint16_t)~0", 0xFFFF);
	EXPECT_UTERM("(unsigned)(uint16_t)~0", 0xFFFF);
	EXPECT_UTERM("(uint8_t) (uint16_t)~0", 0xFF);
	EXPECT_UTERM("(uint16_t)(uint16_t)~0", 0xFFFF);
	EXPECT_UTERM("(uint32_t)(uint16_t)~0", 0xFFFF);
	EXPECT_UTERM("(uint64_t)(uint16_t)~0", 0xFFFF);

	TEST_TERM_TYPE("(int32_t) -128", "-128");
	EXPECT_ITERM("(int)    (int32_t)-128", -128);
	EXPECT_ITERM("(int8_t) (int32_t)-128", -128);
	EXPECT_ITERM("(int16_t)(int32_t)-128", -128);
	EXPECT_ITERM("(int32_t)(int32_t)-128", -128);
	EXPECT_ITERM("(int64_t)(int32_t)-128", -128);
	EXPECT_UTERM("(unsigned)(int32_t)0x7FFF", 0x7FFF);
	EXPECT_UTERM("(uint8_t) (int32_t)0x7FFF", 0xFF);
	EXPECT_UTERM("(uint16_t)(int32_t)0x7FFF", 0x7FFF);
	EXPECT_UTERM("(uint32_t)(int32_t)0x7FFF", 0x7FFF);
	EXPECT_UTERM("(uint64_t)(int32_t)0x7FFF", 0x7FFF);
	EXPECT_MSTR(EVAL("(int32_t) -12345"), "-12345");

	TEST_TERM_TYPE("(uint32_t) 128", "128");
	EXPECT_ITERM("(int)    (uint32_t)~0", -1);
	EXPECT_ITERM("(int8_t) (uint32_t)~0", -1);
	EXPECT_ITERM("(int16_t)(uint32_t)~0", -1);
	EXPECT_ITERM("(int32_t)(uint32_t)~0", -1);
	EXPECT_ITERM("(int64_t)(uint32_t)~0", 0xFFFFFFFFll);
	EXPECT_UTERM("(unsigned)(uint32_t)~0", 0xFFFFFFFFull);
	EXPECT_UTERM("(uint8_t) (uint32_t)~0", 0xFF);
	EXPECT_UTERM("(uint16_t)(uint32_t)~0", 0xFFFF);
	EXPECT_UTERM("(uint32_t)(uint32_t)~0", 0xFFFFFFFFull);
	EXPECT_UTERM("(uint64_t)(uint32_t)~0", 0xFFFFFFFFull);
	EXPECT_MSTR(EVAL("(uint32_t) 12345"), "12345");

	TEST_TERM_TYPE("(int64_t) -128", "-128");
	EXPECT_ITERM("(int)    (int64_t)-128", -128);
	EXPECT_ITERM("(int8_t) (int64_t)-128", -128);
	EXPECT_ITERM("(int16_t)(int64_t)-128", -128);
	EXPECT_ITERM("(int32_t)(int64_t)-128", -128);
	EXPECT_ITERM("(int64_t)(int64_t)-128", -128);
	EXPECT_UTERM("(unsigned)(int64_t)0x7FFF", 0x7FFF);
	EXPECT_UTERM("(uint8_t) (int64_t)0x7FFF", 0xFF);
	EXPECT_UTERM("(uint16_t)(int64_t)0x7FFF", 0x7FFF);
	EXPECT_UTERM("(uint32_t)(int64_t)0x7FFF", 0x7FFF);
	EXPECT_UTERM("(uint64_t)(int64_t)0x7FFF", 0x7FFF);
	EXPECT_MSTR(EVAL("(int64_t) -1234567890"), "-1234567890");

	TEST_TERM_TYPE("(uint64_t) 128", "128");
	EXPECT_ITERM("(int)    (uint64_t)~0", -1);
	EXPECT_ITERM("(int8_t) (uint64_t)~0", -1);
	EXPECT_ITERM("(int16_t)(uint64_t)~0", -1);
	EXPECT_ITERM("(int32_t)(uint64_t)~0", -1);
	EXPECT_ITERM("(int64_t)(uint64_t)~0", -1);
	EXPECT_UTERM("(unsigned)(uint64_t)~0", 0xFFFFFFFFull);
	EXPECT_UTERM("(uint8_t) (uint64_t)~0", 0xFF);
	EXPECT_UTERM("(uint16_t)(uint64_t)~0", 0xFFFF);
	EXPECT_UTERM("(uint32_t)(uint64_t)~0", 0xFFFFFFFFull);
	EXPECT_UTERM("(uint64_t)(uint64_t)~0", 0xFFFFFFFFFFFFFFFFull);
	EXPECT_MSTR(EVAL("(uint64_t) 1234567890"), "1234567890");

	TEST_OBJ_TYPE(varint2Obj(-12345), "-12345");
	EXPECT_ITERM("(int)    -128l", -128);
	EXPECT_ITERM("(int8_t) -128l", -128);
	EXPECT_ITERM("(int16_t)-128l", -128);
	EXPECT_ITERM("(int32_t)-128l", -128);
	EXPECT_ITERM("(int64_t)-128l", -128);
	EXPECT_UTERM("(unsigned)0x7FFFl", 0x7FFF);
	EXPECT_UTERM("(uint8_t) 0x7FFFl", 0xFF);
	EXPECT_UTERM("(uint16_t)0x7FFFl", 0x7FFF);
	EXPECT_UTERM("(uint32_t)0x7FFFl", 0x7FFF);
	EXPECT_UTERM("(uint64_t)0x7FFFl", 0x7FFF);
	EXPECT_MSTR(EVAL("-1234567890l"), "-1234567890");

	TEST_OBJ_TYPE(varsize2Obj(42), "42");
	EXPECT_ITERM("(int)    ~0ul", -1);
	EXPECT_ITERM("(int8_t) ~0ul", -1);
	EXPECT_ITERM("(int16_t)~0ul", -1);
	EXPECT_ITERM("(int32_t)~0ul", -1);
	EXPECT_ITERM("(int64_t)~0ul", -1);
	EXPECT_UTERM("(unsigned)~0ul", 0xFFFFFFFFull);
	EXPECT_UTERM("(uint8_t) ~0ul", 0xFF);
	EXPECT_UTERM("(uint16_t)~0ul", 0xFFFF);
	EXPECT_UTERM("(uint32_t)~0ul", 0xFFFFFFFFull);
	EXPECT_UTERM("(uint64_t)~0ul", 0xFFFFFFFFFFFFFFFFull);
	EXPECT_MSTR(EVAL("1234567890ul"), "1234567890");

	EXPECT_ITERM("abs(-2)", 2);
	EXPECT_ITERM("abs(-32u)", -32u);
	EXPECT_ITERM("min(3, 5)", 3);
	EXPECT_ITERM("max(3, 5)", 5);
	EXPECT_ITERM("cmp(2, 5)", -1);
	EXPECT_ITERM("cmp(2, 2)", 0);
	EXPECT_ITERM("rcmp(2, 5)", 1);
	EXPECT_ITERM("rcmp(5, 5)", 0);

	EXPECT_DTERM("(float) 3", 3.);
	EXPECT_DTERM("(double) 3", 3.);

	EXPECT_ITERM("(bool) 5", 1);
	EXPECT_ITERM("!5", 0);
	EXPECT_ITERM("~0", -1);
	EXPECT_ITERM("-(-5)", 5);
	EXPECT_ITERM("+(-5)", -5);
	EXPECT_ITERM("(int) 3.5", 3);

	EXPECT_ITERM("3 * 5", 15);
	EXPECT_ITERM("5 / 2", 2);
	EXPECT_ITERM("5 % 2", 1);
	EXPECT_ITERM("2 + 5", 7);
	EXPECT_ITERM("5 - 7", -2);

	EXPECT_ITERM("1 << 2", (1 << 2));
	EXPECT_ITERM("64 >> 2", (64 >> 2));
	EXPECT_ITERM("0xF0 & 0x1F", 0x10);
	EXPECT_ITERM("123 ^ 456", (123 ^ 456));
	EXPECT_ITERM("0xF0 | 0x03", 0xF3);

	EXPECT_ITERM("2 < 5", 1);
	EXPECT_ITERM("2 <= 5", 1);
	EXPECT_ITERM("2 == 5", 0);
	EXPECT_ITERM("2 != 5", 1);
	EXPECT_ITERM("2 > 5", 0);
	EXPECT_ITERM("2 >= 5", 0);

	PushVarTab(NULL, NULL);
	EXPECT_ITERM("int x = 5", 5);
	EXPECT_ITERM("x++", 5);
	EXPECT_ITERM("++x", 7);
	EXPECT_ITERM("--x", 6);
	EXPECT_ITERM("x--", 6);
	EXPECT_ITERM("x *= 2", 10);
	EXPECT_ITERM("x /= 3", 3);
	EXPECT_ITERM("x %= 2", 1);
	EXPECT_ITERM("x += 6", 7);
	EXPECT_ITERM("x -= 2", 5);
	EXPECT_ITERM("x |= 2", 7);
	EXPECT_ITERM("x &= 12", 4);
	EXPECT_ITERM("x ^= 2", 6);
	EXPECT_ITERM("x <<= 1", 12);
	EXPECT_ITERM("x >>= 2", 3);
	PopVarTab();
}

TEST_DECL(stdfloat)
{
	TEST_TERM_TYPE("float(3.5)", "3.5000000");
	TEST_OBJ_TYPE(double2Obj(3.5), "3.500000000000000");

	EXPECT_DTERM("abs(-2.)", 2.);
	EXPECT_DTERM("min(3., 5.)", 3.);
	EXPECT_DTERM("max(3., 5.)", 5.);

	EXPECT_ITERM("(bool) 5.", 1);
	EXPECT_ITERM("!5.", 0);
	EXPECT_DTERM("-(-5.)", 5.);
	EXPECT_DTERM("+(-5.)", -5.);

	EXPECT_DTERM("3. * 5.", 15.);
	EXPECT_DTERM("5. / 2.", 2.5);
	EXPECT_DTERM("2. + 5.5", 7.5);
	EXPECT_DTERM("5.25 - 7.", -1.75);

	EXPECT_ITERM("2.25 < 2.5", 1);
	EXPECT_ITERM("2.25 <= 2.5", 1);
	EXPECT_ITERM("2.25 == 2.5", 0);
	EXPECT_ITERM("2.25 != 2.5", 1);
	EXPECT_ITERM("2.25 > 2.5", 0);
	EXPECT_ITERM("2.25 >= 2.5", 0);

	PushVarTab(NULL, NULL);
	EXPECT_DTERM("double x = 5.5", 5.5);
	EXPECT_DTERM("x++", 5.5);
	EXPECT_DTERM("++x", 7.5);
	EXPECT_DTERM("--x", 6.5);
	EXPECT_DTERM("x--", 6.5);
	EXPECT_DTERM("x *= 2.", 11.);
	EXPECT_DTERM("x /= 4.", 2.75);
	EXPECT_DTERM("x += 6.", 8.75);
	EXPECT_DTERM("x -= 2.5", 6.25);
	PopVarTab();

	EXPECT_DTERM("xdiv(5., 0.)", 0.);
	EXPECT_DTERM("xdiv(5., 2.)", 2.5);
	EXPECT_DTERM("xdiv1(5., 0.)", 1.);
	EXPECT_DTERM("xdiv1(5., 2.)", 2.5);
	EXPECT_DTERM("idx(5., 20.)", 25.);
	EXPECT_DTERM("idx(5., 0.)", 0.);
	EXPECT_DTERM("rate(8., 10.)", -20.);
	EXPECT_DTERM("rate(12., 10.)", 20.);
	EXPECT_DTERM("rate(10., 0.)", 0.);
	EXPECT_DTERM("rnd(2.4)", 2.);
	EXPECT_DTERM("rnd(2.5)", 3.);
	EXPECT_DTERM("rnd(-2.5)", -3.);
}

TEST_DECL(math)
{
	EXPECT_DTERM("sqrt(25.)", 5.);
	EXPECT_DTERM("log(2.)", log(2.));
	EXPECT_DTERM("xlog(-2.)", 0.);
	EXPECT_DTERM("exp(1.)", exp(1.));
	EXPECT_DTERM("ceil(2.1)", 3.);
	EXPECT_DTERM("floor(-3.2)", -4.);
	EXPECT_DTERM("sin(1.)", sin(1.));
	EXPECT_DTERM("cos(1.)", cos(1.));
	EXPECT_DTERM("tan(1.)", tan(1.));
	EXPECT_DTERM("asin(0.5)", asin(0.5));
	EXPECT_DTERM("acos(0.5)", acos(0.5));
	EXPECT_DTERM("atan(1.0)", atan(1.0));
	EXPECT_DTERM("atan2(-1.0, -1.0)", atan2(-1.0, -1.0));
	EXPECT_DTERM("pow(2.0, 0.5)", pow(2.0, 0.5));
	EXPECT_DTERM("3.8 % 3.5", fmod(3.8, 3.5));
	EXPECT_DTERM("rnd125(0.)", 0.);
	EXPECT_DTERM("rnd125(-0.21)", -0.5);
	EXPECT_DTERM("rnd125(7.8)", 10.);
	EXPECT_DTERM("ldexp(10., 4)", ldexp(10., 4));
	PushVarTab(NULL, NULL);
	EXPECT_ITERM("int n = 0", 0);
	EXPECT_DTERM("double x = 0.", 0.);
	EXPECT_DTERM("modf(3.75, x)", 0.75);
	EXPECT_DTERM("x", 3.0);
	EXPECT_DTERM("frexp(2.5, n)", 2.5 / 4.);
	EXPECT_ITERM("n", 2);
	PopVarTab();
}

TEST_SUITE
{
	SetupStd();
	SetupMath();
	TEST_RUN(base);
	TEST_RUN(stdint);
	TEST_RUN(stdfloat);
	TEST_RUN(math);
	CLEANUP_MSTR;
	CLEANUP_OBJ;
}
