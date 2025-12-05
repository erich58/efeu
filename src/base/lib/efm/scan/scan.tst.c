#include <EFEU/UnitTest.h>
#include <EFEU/ioscan.h>
#include <EFEU/locale.h>
#include <EFEU/mstring.h>
#include <EFEU/utest/out.tst.h>

static int testkey(const char *str, const char *key)
{
	IO *io = io_cstr(str);
	int rval = io_testkey(io, key);
	io_copy(io, OUT());
	io_close(io);
	return rval;
}

static int valscan(const char *str, unsigned flags, void **data)
{
	IO *io = io_cstr(str);
	utest_out();
	int rval = io_valscan(io, flags, data);
	io_copy(io, OUT());
	io_close(io);
	return rval;
}

static int scan(const char *str, unsigned flags, void **data)
{
	IO *io = io_cstr(str);
	int rval = io_scan(io, flags, data);
	io_copy(io, OUT());
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
	EXPECT(C_strtod("3.125,8", &p) == 3.125);
	EXPECT_STR(p, ",8");
	setlocale(LC_ALL, "C");
	EXPECT(C_strtod("3.125,8", &p) == 3.125);
	EXPECT_STR(p, ",8");
}

TEST_DECL(mstr2int64)
{
	char *p;
	EXPECT(mstr2int64(" -123", NULL, 0) == -123ll);
	EXPECT(mstr2int64(" +123", NULL, 0) == 123ll);
	EXPECT(mstr2uint64(" +0123", NULL, 0) == 0123ll);
	EXPECT(mstr2uint64(" -1", NULL, 0) == ~0ull);

	EXPECT(mstr2int64("1234567890.", &p, 0) == 1234567890ll);
	EXPECT_STR(p, ".");
	EXPECT(mstr2int64("0xABCDEF", &p, 0) == 0xABCDEFll);
	EXPECT(mstr2int64("0xabcdefg", &p, 0) == 0xABCDEFll);
	EXPECT_STR(p, "g");
	EXPECT(mstr2int64("0b1012", &p, 0) == 5ll);
	EXPECT_STR(p, "2");
	EXPECT(mstr2int64("012345678", &p, 0) == 01234567ll);
	EXPECT_STR(p, "8");

	/* Overflow */
	EXPECT(mstr2uint64("ffffffffffffffffee", &p, 16) == 0xffffffffffffffffll);
	EXPECT(mstr2int64("ffffffffffffffff", &p, 16) == 0x7fffffffffffffffll);

	/* Coverage */
	EXPECT(mstr2int64("ghijklmnop", &p, 25) != 0);
	EXPECT_STR(p, "p");
	EXPECT(mstr2int64("pqrstuvwxyz.", &p, 36) != 0);
	EXPECT_STR(p, ".");
}

TEST_DECL(testkey)
{
	EXPECT(testkey("abc:", NULL) == 0);
	EXPECT_OUT("abc:");
	EXPECT(testkey("abc:", "abc") == 1);
	EXPECT_OUT(":");
	EXPECT(testkey("abcd:", "abc") == 0);
	EXPECT_OUT("abcd:");
}

TEST_DECL(valscan)
{
	void *p;

	if	(EXPECT(valscan(" 3.5", 0, &p) == SCAN_DOUBLE))
		EXPECT(*((double *) p) == 3.5); 

	if	(EXPECT(valscan(" -.5", 0, &p) == SCAN_DOUBLE))
		EXPECT(*((double *) p) == -0.5); 

	if	(EXPECT(valscan(" 2.5e-2", 0, &p) == SCAN_DOUBLE))
		EXPECT(*((double *) p) == 2.5e-2); 

	if	(EXPECT(valscan(" 3.5", SCAN_INT, &p) == SCAN_INT))
		EXPECT(*((int *) p) == 3); 

	EXPECT_OUT(".5");

	if	(EXPECT(valscan(" 3", SCAN_DOUBLE, &p) == SCAN_DOUBLE))
		EXPECT(*((double *) p) == 3.); 

	if	(EXPECT(valscan(" 010", 0, &p) == SCAN_INT))
		EXPECT(*((int *) p) == 10); 

	if	(EXPECT(valscan(" 010", SCAN_ANYVAL, &p) == SCAN_INT))
		EXPECT(*((int *) p) == 8); 

	if	(EXPECT(valscan(" 0xabcdefgh", SCAN_ANYVAL, &p) == SCAN_INT))
		EXPECT(*((int *) p) == 0xabcdef); 

	EXPECT_OUT("gh");

	if	(EXPECT(valscan("0x1234567890ll", SCAN_ANYVAL, &p) == SCAN_INT64))
		EXPECT(*((int64_t *) p) == 0x1234567890ll); 


	if	(EXPECT(valscan(" 0b1012", SCAN_ANYVAL, &p) == SCAN_INT))
		EXPECT(*((int *) p) == 5); 

	EXPECT_OUT("2");

	if	(EXPECT(valscan(" -1u", 0, &p) == (SCAN_INT|SCAN_UNSIGNED)))
		EXPECT(*((unsigned *) p) == ~0); 

	if	(EXPECT(valscan(" 5ll", 0, &p) == SCAN_INT64))
		EXPECT(*((int64_t *) p) == 5); 
}

TEST_DECL(scan)
{
	void *p;

	EXPECT(io_getname(NULL) == NULL);
	EXPECT(io_scan(NULL, SCAN_ANYTYPE, NULL) == 0);

	EXPECT(scan(" \t123", 0, NULL) == 0);
	EXPECT_OUT("123");

	EXPECT(scan("3.5", SCAN_ANYTYPE, NULL) == SCAN_DOUBLE);
	EXPECT(scan("3", SCAN_ANYTYPE, NULL) == SCAN_INT);
	EXPECT(scan("3ll", SCAN_ANYTYPE, NULL) == SCAN_INT64);
	EXPECT(scan("3ull", SCAN_ANYTYPE, NULL) == (SCAN_UNSIGNED|SCAN_INT64));
	EXPECT(scan("'a'", SCAN_ANYTYPE, NULL) == SCAN_CHAR);
	EXPECT(scan("L'a'", SCAN_ANYTYPE, NULL) == SCAN_UCS);
	EXPECT(scan("\"a\"", SCAN_ANYTYPE, NULL) == SCAN_STR);
	EXPECT(scan("\"\"", SCAN_ANYTYPE, NULL) == SCAN_STR);

	if	(EXPECT(scan("'a'", SCAN_XCHAR, &p) == SCAN_CHAR))
		EXPECT(*((unsigned char *) p) == 'a'); 
	if	(EXPECT(scan("L'a'", SCAN_XCHAR, &p) == SCAN_UCS))
		EXPECT(*((int32_t *) p) == 'a'); 
	if	(EXPECT(scan("'Ä'", SCAN_XCHAR, &p) == SCAN_UCS))
		EXPECT(*((int32_t *) p) == 0xc4); 
	if	(EXPECT(scan("'\xc3\x84'", SCAN_XCHAR, &p) == SCAN_UCS))
		EXPECT(*((int32_t *) p) == 0xc4); 
	if	(EXPECT(scan("'€'", SCAN_XCHAR, &p) == SCAN_UCS))
		EXPECT(*((int32_t *) p) == 0x20ac); 
	if	(EXPECT(scan("'\342\202\254'", SCAN_XCHAR, &p) == SCAN_UCS))
		EXPECT(*((int32_t *) p) == 0x20ac); 

	EXPECT(scan_char("'\\''") == '\'');
	EXPECT(scan_char("'\\\\'") == '\\');
	EXPECT(scan_char("'\\n'") == '\n');
	EXPECT(scan_char("'\\t'") == '\t');
	EXPECT(scan_char("'\\r'") == '\r');
	EXPECT(scan_char("'\\f'") == '\f');
	EXPECT(scan_char("'\\xab'") == 0xab);
	EXPECT(scan_char("'\231'") == 0231);
	EXPECT(scan_char("'\\231'") == 0231);
	EXPECT(scan_char("'\xc4'") == 0xc4);
	EXPECT(scan_char("'\\xc4'") == 0xc4);
	EXPECT(scan_char("'\xc0\x8a'") == 0x0a);
	EXPECT(scan_char("'\xe0\x8a\x8b'") == (0xa << 6) + 0xb);

	if	(EXPECT(scan("NULL", SCAN_ANYTYPE, &p) == SCAN_NAME))
		EXPECT_STR(p, NULL); 

	if	(EXPECT(scan("NULL", SCAN_STR, &p) == SCAN_STR))
		EXPECT_STR(p, NULL); 

	if	(EXPECT(scan("NULLER", SCAN_ANYTYPE, &p) == SCAN_NAME))
		EXPECT_STR(p, "NULLER"); 

	if	(EXPECT(scan("\"abc\"", SCAN_ANYTYPE, &p) == SCAN_STR))
	{
		EXPECT_STR(p, "abc"); 
		memfree(p);
	}

	if	(EXPECT(scan("LANG", SCAN_ANYTYPE, &p) == SCAN_NAME))
	{
		EXPECT_STR(p, "LANG"); 
		memfree(p);
	}
}

TEST_DECL(locale)
{
	void *p;

	PushLocale();

	if	(EXPECT(valscan("123.456,78", 0, &p) == SCAN_DOUBLE))
		EXPECT(*((double *) p) == 123.456); 

	EXPECT_OUT(",78");

	SetLocale(LOC_SCAN, "de");

	if	(EXPECT(valscan("123.456,78", 0, &p) == SCAN_DOUBLE))
		EXPECT(*((double *) p) == 123456.78); 

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
	CLEANUP_OUT;
}
