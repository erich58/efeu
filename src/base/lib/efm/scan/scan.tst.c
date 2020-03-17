#include <EFEU/UnitTest.h>
#include <EFEU/ioscan.h>
#include <EFEU/locale.h>
#include <EFEU/mstring.h>
#include "utest/data.tst.h"

static int testkey(const char *str, const char *key)
{
	IO *io = io_cstr(str);
	int rval = io_testkey(io, key);
	io_copy(io, out());
	io_close(io);
	return rval;
}

static int valscan(const char *str, unsigned flags, void **data)
{
	IO *io = io_cstr(str);
	int rval = io_valscan(io, flags, data);
	io_copy(io, out());
	io_close(io);
	return rval;
}

static int scan(const char *str, unsigned flags, void **data)
{
	IO *io = io_cstr(str);
	int rval = io_scan(io, flags, data);
	io_copy(io, out());
	io_close(io);
	return rval;
}

static int32_t scan_char(const char *str)
{
	void *p;

	switch (scan(str, SCAN_XCHAR, &p))
	{
	case SCAN_CHAR:	return *((unsigned char *) p);
	case SCAN_UCS:	return *((int32_t *) p);
	default:	return 0;
	}
}

TEST_DECL(C_strtod)
{
	char *p;
	PRINT_STR(setlocale(LC_ALL, ""));
	EXPECT_TRUE(C_strtod("3.125,8", &p) == 3.125);
	EXPECT_STR(p, ",8");
	setlocale(LC_ALL, "C");
	EXPECT_TRUE(C_strtod("3.125,8", &p) == 3.125);
	EXPECT_STR(p, ",8");
}

TEST_DECL(mstr2int64)
{
	char *p;
	EXPECT_TRUE(mstr2int64(" -123", NULL, 0) == -123ll);
	EXPECT_TRUE(mstr2int64(" +123", NULL, 0) == 123ll);
	EXPECT_TRUE(mstr2uint64(" +0123", NULL, 0) == 0123ll);
	EXPECT_TRUE(mstr2uint64(" -1", NULL, 0) == ~0ull);

	EXPECT_TRUE(mstr2int64("1234567890.", &p, 0) == 1234567890ll);
	EXPECT_STR(p, ".");
	EXPECT_TRUE(mstr2int64("0xABCDEF", &p, 0) == 0xABCDEFll);
	EXPECT_TRUE(mstr2int64("0xabcdefg", &p, 0) == 0xABCDEFll);
	EXPECT_STR(p, "g");
	EXPECT_TRUE(mstr2int64("0b1012", &p, 0) == 5ll);
	EXPECT_STR(p, "2");
	EXPECT_TRUE(mstr2int64("012345678", &p, 0) == 01234567ll);
	EXPECT_STR(p, "8");

	/* Overflow */
	EXPECT_TRUE(mstr2uint64("ffffffffffffffffee", &p, 16) == 0xffffffffffffffffll);
	EXPECT_TRUE(mstr2int64("ffffffffffffffff", &p, 16) == 0x7fffffffffffffffll);

	/* Coverage */
	EXPECT_TRUE(mstr2int64("ghijklmnop", &p, 25) != 0);
	EXPECT_STR(p, "p");
	EXPECT_TRUE(mstr2int64("pqrstuvwxyz.", &p, 36) != 0);
	EXPECT_STR(p, ".");
}

TEST_DECL(testkey)
{
	EXPECT_TRUE(testkey("abc:", NULL) == 0);
	EXPECT_STR(data(), "abc:");
	EXPECT_TRUE(testkey("abc:", "abc") == 1);
	EXPECT_STR(data(), ":");
	EXPECT_TRUE(testkey("abcd:", "abc") == 0);
	EXPECT_STR(data(), "abcd:");
}

TEST_DECL(valscan)
{
	void *p;

	if	(EXPECT_TRUE(valscan(" 3.5", 0, &p) == SCAN_DOUBLE))
		EXPECT_TRUE(*((double *) p) == 3.5); 

	if	(EXPECT_TRUE(valscan(" -.5", 0, &p) == SCAN_DOUBLE))
		EXPECT_TRUE(*((double *) p) == -0.5); 

	if	(EXPECT_TRUE(valscan(" 2.5e-2", 0, &p) == SCAN_DOUBLE))
		EXPECT_TRUE(*((double *) p) == 2.5e-2); 

	if	(EXPECT_TRUE(valscan(" 3.5", SCAN_INT, &p) == SCAN_INT))
		EXPECT_TRUE(*((int *) p) == 3); 

	EXPECT_STR(data(), ".5");

	if	(EXPECT_TRUE(valscan(" 3", SCAN_DOUBLE, &p) == SCAN_DOUBLE))
		EXPECT_TRUE(*((double *) p) == 3.); 

	if	(EXPECT_TRUE(valscan(" 010", 0, &p) == SCAN_INT))
		EXPECT_TRUE(*((int *) p) == 10); 

	if	(EXPECT_TRUE(valscan(" 010", SCAN_ANYVAL, &p) == SCAN_INT))
		EXPECT_TRUE(*((int *) p) == 8); 

	if	(EXPECT_TRUE(valscan(" 0xabcdefgh", SCAN_ANYVAL, &p) == SCAN_INT))
		EXPECT_TRUE(*((int *) p) == 0xabcdef); 

	EXPECT_STR(data(), "gh");

	if	(EXPECT_TRUE(valscan("0x1234567890ll", SCAN_ANYVAL, &p) == SCAN_INT64))
		EXPECT_TRUE(*((int64_t *) p) == 0x1234567890ll); 


	if	(EXPECT_TRUE(valscan(" 0b1012", SCAN_ANYVAL, &p) == SCAN_INT))
		EXPECT_TRUE(*((int *) p) == 5); 

	EXPECT_STR(data(), "2");

	if	(EXPECT_TRUE(valscan(" -1u", 0, &p) == (SCAN_INT|SCAN_UNSIGNED)))
		EXPECT_TRUE(*((unsigned *) p) == ~0); 

	if	(EXPECT_TRUE(valscan(" 5ll", 0, &p) == SCAN_INT64))
		EXPECT_TRUE(*((int64_t *) p) == 5); 
}

TEST_DECL(scan)
{
	void *p;

	EXPECT_TRUE(io_getname(NULL) == NULL);
	EXPECT_TRUE(io_scan(NULL, SCAN_ANYTYPE, NULL) == 0);

	EXPECT_TRUE(scan(" \t123", 0, NULL) == 0);
	EXPECT_STR(data(), "123");

	EXPECT_TRUE(scan("3.5", SCAN_ANYTYPE, NULL) == SCAN_DOUBLE);
	EXPECT_TRUE(scan("3", SCAN_ANYTYPE, NULL) == SCAN_INT);
	EXPECT_TRUE(scan("3ll", SCAN_ANYTYPE, NULL) == SCAN_INT64);
	EXPECT_TRUE(scan("3ull", SCAN_ANYTYPE, NULL) == (SCAN_UNSIGNED|SCAN_INT64));
	EXPECT_TRUE(scan("'a'", SCAN_ANYTYPE, NULL) == SCAN_CHAR);
	EXPECT_TRUE(scan("\"a\"", SCAN_ANYTYPE, NULL) == SCAN_STR);
	EXPECT_TRUE(scan("L'a'", SCAN_ANYTYPE, NULL) == SCAN_UCS);
	EXPECT_TRUE(scan("L\"a\"", SCAN_ANYTYPE, NULL) == SCAN_STR);
	EXPECT_TRUE(scan("L'a'", 0, NULL) == 0);
	EXPECT_STR(data(), "L'a'");
	EXPECT_TRUE(scan("L'a'", SCAN_NAME, NULL) == SCAN_NAME);
	EXPECT_STR(data(), "'a'");
	EXPECT_TRUE(scan("L'a'", SCAN_UCS, NULL) == SCAN_UCS);
	EXPECT_TRUE(scan("L 'a'", SCAN_UCS, NULL) == 0);
	EXPECT_STR(data(), "L 'a'");
	EXPECT_TRUE(scan("L 'a'", SCAN_ANYTYPE, NULL) == SCAN_NAME);

	if	(EXPECT_TRUE(scan("'a'", SCAN_XCHAR, &p) == SCAN_CHAR))
		EXPECT_TRUE(*((unsigned char *) p) == 'a'); 
	if	(EXPECT_TRUE(scan("L'a'", SCAN_XCHAR, &p) == SCAN_UCS))
		EXPECT_TRUE(*((int32_t *) p) == 'a'); 
	if	(EXPECT_TRUE(scan("'Ä'", SCAN_XCHAR, &p) == SCAN_CHAR))
		EXPECT_TRUE(*((unsigned char *) p) == 0xc4); 
	if	(EXPECT_TRUE(scan("'\xc3\x84'", SCAN_XCHAR, &p) == SCAN_CHAR))
		EXPECT_TRUE(*((unsigned char *) p) == 0xc4); 
	if	(EXPECT_TRUE(scan("'€'", SCAN_XCHAR, &p) == SCAN_UCS))
		EXPECT_TRUE(*((int32_t *) p) == 0x20ac); 
	if	(EXPECT_TRUE(scan("'\\342\\202\\254'", SCAN_XCHAR, &p) == SCAN_UCS))
		EXPECT_TRUE(*((int32_t *) p) == 0x20ac); 

	EXPECT_TRUE(scan_char("'\\''") == '\'');
	EXPECT_TRUE(scan_char("'\\\\'") == '\\');
	EXPECT_TRUE(scan_char("'\\n'") == '\n');
	EXPECT_TRUE(scan_char("'\\t'") == '\t');
	EXPECT_TRUE(scan_char("'\\r'") == '\r');
	EXPECT_TRUE(scan_char("'\\f'") == '\f');
	EXPECT_TRUE(scan_char("'\\xab'") == 0xab);
	EXPECT_TRUE(scan_char("'\231'") == 0231);
	EXPECT_TRUE(scan_char("'\\231'") == 0231);
	EXPECT_TRUE(scan_char("'\xc4'") == 0xc4);
	EXPECT_TRUE(scan_char("'\\xc4'") == 0xc4);
	EXPECT_TRUE(scan_char("'\xc0\x8a'") == 0x0a);
	EXPECT_TRUE(scan_char("'\\xc0\\x8a'") == 0x0a);
	EXPECT_TRUE(scan_char("'\xe0\x8a\x8b'") == (0xa << 6) + 0xb);
	EXPECT_TRUE(scan_char("'\\xe0\\x8a\\x8b'") == (0xa << 6) + 0xb);

	if	(EXPECT_TRUE(scan("NULL", SCAN_ANYTYPE, &p) == SCAN_NAME))
		EXPECT_STR(p, NULL); 

	if	(EXPECT_TRUE(scan("NULL", SCAN_STR, &p) == SCAN_STR))
		EXPECT_STR(p, NULL); 

	if	(EXPECT_TRUE(scan("\"abc\"", SCAN_ANYTYPE, &p) == SCAN_STR))
		EXPECT_STR(cstr(p), "abc"); 
}

TEST_DECL(locale)
{
	void *p;

	PushLocale();

	if	(EXPECT_TRUE(valscan("123.456,78", 0, &p) == SCAN_DOUBLE))
		EXPECT_TRUE(*((double *) p) == 123.456); 

	EXPECT_STR(data(), ",78");

	SetLocale(LOC_SCAN, "de");

	if	(EXPECT_TRUE(valscan("123.456,78", 0, &p) == SCAN_DOUBLE))
		EXPECT_TRUE(*((double *) p) == 123456.78); 

	PopLocale();
}

TEST_SUITE
{
	TEST_RUN(C_strtod);
	TEST_RUN(mstr2int64);
	TEST_RUN(testkey);
	TEST_RUN(valscan);
	TEST_RUN(scan);
	TEST_RUN(locale);
}
