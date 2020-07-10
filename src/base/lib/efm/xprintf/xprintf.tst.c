#include <EFEU/UnitTest.h>
#include <EFEU/fmtkey.h>
#include <EFEU/locale.h>
#include <EFEU/math.h>

static int (*do_vprintf) (StrBuf *buf, const char *fmt, va_list args);

static FmtKey *getfmt(const char *def)
{
	return fmtkey(NULL, fmtkey_pgetc, &def);
}

static const char *fmt(const char *fmt, ...)
{
	static STRBUF(buf, 256);

	if	(fmt)
	{
		va_list args;

		sb_clean(&buf);
		va_start(args, fmt);
		do_vprintf(&buf, fmt, args);
		va_end(args);
		return sb_nul(&buf);
	}

	return NULL;
}

TEST_DECL(getc)
{
	char *str = "Äa";
	char *p = str;
	EXPECT(fmtkey_pgetc(&p) == 0xc3);
	EXPECT(fmtkey_pgetc(&p) == 0x84);
	EXPECT(fmtkey_pgetc(&p) == 'a');
	EXPECT(fmtkey_pgetc(&p) == EOF);
	p = str;
	EXPECT(fmtkey_pgetucs(&p) == 0xc4);
	EXPECT(fmtkey_pgetucs(&p) == 'a');
	EXPECT(fmtkey_pgetc(&p) == EOF);
}

TEST_DECL(sb_printf)
{
	StrBuf *sb = sb_create(2);
	EXPECT(sb_printf(sb, NULL) == 0);
	EXPECT(sb_printf(NULL, "%d", 42) == 0);
	EXPECT(sb_printf(sb, "%d", 42) == 2);
	EXPECT_STR(sb_nul(sb), "42");
	EXPECT(sb_xprintf(sb, "Ä") == 2);
	sb_destroy(sb);
}

TEST_DECL(sb_xprintf)
{
	StrBuf *sb = sb_create(2);
	EXPECT(sb_xprintf(sb, NULL) == 0);
	EXPECT(sb_xprintf(NULL, "%d", 42) == 0);
	EXPECT(sb_xprintf(sb, "%d", 42) == 2);
	EXPECT_STR(sb_nul(sb), "42");
	EXPECT(sb_xprintf(sb, "Ä") == 2);
	sb_destroy(sb);
}

TEST_DECL(sb_mbprintf)
{
	StrBuf *sb = sb_create(2);
	EXPECT(sb_mbprintf(sb, NULL) == 0);
	EXPECT(sb_mbprintf(NULL, "%d", 42) == 0);
	EXPECT(sb_mbprintf(sb, "%d", 42) == 2);
	EXPECT_STR(sb_nul(sb), "42");
	EXPECT(sb_mbprintf(sb, "Ä") == 1);
	sb_destroy(sb);
}

TEST_DECL(fmt)
{
	EXPECT_STR(fmt("|%%|"), "|%|");
	EXPECT_STR(fmt("|%4d|%04d|%+4d|", 42, 42, 42), "|  42|0042| +42|");
	EXPECT_STR(fmt("|%.3d|", 1), "|001|");
	EXPECT_STR(fmt("|%0d|", 1), "|1|");
	EXPECT_STR(fmt("|%*.*d|%*d|", 5, 3, 1, -4, 2), "|  001|2   |");
	EXPECT_STR(fmt("|%5.3o|%-5.1o|%#5o|", 1, 2, 3), "|  001|2    |   03|");
	EXPECT_STR(fmt("|%4.3x|%-4.1x|%#4x|", 10, 11, 12), "| 00a|b   | 0xc|");
	EXPECT_STR(fmt("|%hhx|%hx|", ~0, ~0), "|ff|ffff|");
	EXPECT_STR(fmt("|%zd|", (size_t) 12345), "|12345|");
	EXPECT_STR(fmt("|%jd|", (intmax_t) 12345), "|12345|");
	EXPECT_STR(fmt("|%td|", (ptrdiff_t) 12345), "|12345|");
	EXPECT_STR(fmt("|%zd|", (ssize_t) -12345), "|-12345|");
	EXPECT_STR(fmt("|%jd|", (intmax_t) -12345), "|-12345|");
	EXPECT_STR(fmt("|%td|", (ptrdiff_t) -12345), "|-12345|");
	EXPECT_STR(fmt("|%zx|", (ssize_t) 0x1234), "|1234|");
	EXPECT_STR(fmt("|%jx|", (intmax_t) 0x1234), "|1234|");
	EXPECT_STR(fmt("|%tx|", (ptrdiff_t) 0x1234), "|1234|");
	EXPECT_STR(fmt("|%p|", (void *) 0x1234abcd), "|0x1234abcd|");
	EXPECT_STR(fmt("|%*s|", 5, "abc"), "|  abc|");
	EXPECT_STR(fmt("|%*s|", -5, "abc"), "|abc  |");
	EXPECT_STR(fmt("|%*.*s|", -5, 2, "abc"), "|ab   |");
	EXPECT_STR(fmt("|%+7.2f|", 12.345), "| +12.35|");
	EXPECT_STR(fmt("|%7.2f|", -12.345), "| -12.35|");
	EXPECT_STR(fmt("|%5.3g|", 12.345), "| 12.3|");
	EXPECT_STR(fmt("|%g|", 22.222222), "|22.2222|");
}

TEST_DECL(xfmt)
{
	EXPECT_STR(fmt(NULL), NULL);

	EXPECT_STR(fmt("|%hhd|%hd|", 0xFF, 0xFFFF), "|-1|-1|");

	EXPECT_STR(fmt("|%b|%#b|%#B|", 5, 10, 10), "|101|0b1010|0B1010|");
	EXPECT_STR(fmt("|%lx|%llx|", ~0l, ~0ll), "|ffffffff|ffffffffffffffff|");
	EXPECT_STR(fmt("|%Lx|", ~0ll), "|ffffffffffffffff|");
	EXPECT_STR(fmt("|%ld|%lld|", ~0l, ~0ll), "|-1|-1|");
	EXPECT_STR(fmt("|%Ld|", ~0ll), "|-1|");
	EXPECT_STR(fmt("|%P|", (void *) 0x1234abcd), "|0X1234ABCD|");

	EXPECT_STR(fmt("|%5.1f|%5.-1f|", 123.45, 123.45), "|123.5|  120|");
	EXPECT_STR(fmt("|%f|", ExceptionValue(0.)), "|NaN|");
	EXPECT_STR(fmt("|%+5f|", ExceptionValue(1.)), "| +Inf|");
	EXPECT_STR(fmt("|%-5f|", ExceptionValue(-1.)), "|-Inf |");
	EXPECT_STR(fmt("|%8.2e|", 12.345), "| 1.23e+1|");

	float_align = 3;
	EXPECT_STR(fmt("|%8.2e|", 0.00012321), "|  123e-6|");
	EXPECT_STR(fmt("|%#8.2e|", 0.00012321), "| 123.e-6|");
	EXPECT_STR(fmt("|%8.2e|", 0.0012345), "| 1.23e-3|");
	EXPECT_STR(fmt("|%8.2e|", 0.012345), "| 12.3e-3|");
	EXPECT_STR(fmt("|%8.2e|", 0.12321), "|  123e-3|");
	EXPECT_STR(fmt("|%#-8.2e|", 0.12321), "|123.e-3 |");
	EXPECT_STR(fmt("|%8.2e|", 1.2321), "| 1.23e+0|");
	EXPECT_STR(fmt("|%8.2e|", 12.345), "| 12.3e+0|");
	EXPECT_STR(fmt("|%8.2e|", 123.45), "|  123e+0|");
	EXPECT_STR(fmt("|%8.2e|", 1234.5), "| 1.23e+3|");
	EXPECT_STR(fmt("|%8.2e|", 12345.), "| 12.3e+3|");
	EXPECT_STR(fmt("|%8.2e|", 123450.), "|  123e+3|");
	EXPECT_STR(fmt("|%#*.*e|", -8, 2, 123450.), "|123.e+3 |");

	EXPECT_STR(fmt("|%.3g|", 0.00012321), "|123e-6|");
	EXPECT_STR(fmt("|%.3g|", 0.0012321), "|0.00123|");
	EXPECT_STR(fmt("|%.3g|", 0.012321), "|0.0123|");
	EXPECT_STR(fmt("|%.3g|", 0.12321), "|0.123|");
	EXPECT_STR(fmt("|%.3g|", 1.2321), "|1.23|");
	EXPECT_STR(fmt("|%.3g|", 12.321), "|12.3|");
	EXPECT_STR(fmt("|%.3g|", 123.21), "|123|");
	EXPECT_STR(fmt("|%#.3g|", 123.21), "|123.|");
	EXPECT_STR(fmt("|%.3g|", 1232.1), "|1230|");
	EXPECT_STR(fmt("|%#.3g|", 12321.), "|12300.|");
	EXPECT_STR(fmt("|%.3g|", 123210.), "|123000|");
	EXPECT_STR(fmt("|%.3g|", 1232100.), "|1.23e+6|");

	float_align = 1;

	EXPECT_STR(fmt("|%[abc]|%[ä\\x\\]ö]|"), "|abc|ä\\x]ö|");

	EXPECT_STR(fmt("|%c|%lc|", 'a', 0xc4), "|a|Ä|");
	EXPECT_STR(fmt("|%#c|%#lc|", 'a', 0xc4), "|'a'|'Ä'|");
	EXPECT_STR(fmt("|%s|%#s|", NULL, NULL), "||NULL|");
	EXPECT_STR(fmt("|%s|%#s|", "Äbc", "Äbc"), "|Äbc|\"Äbc\"|");

	EXPECT_STR(fmt("|%*.*s|", -5, -2, "abc"), "|bc   |");

	PushLocale();
	EXPECT_STR(fmt("|%10i|", 123456), "|    123456|");
	EXPECT_STR(fmt("|%010i|", 123456), "|0000123456|");
	SetLocale(LOC_PRINT, "de");
	EXPECT_STR(fmt("|%#10i|", 123456), "|   123.456|");
	EXPECT_STR(fmt("|%#010i|", 123456), "|00.123.456|");
	PopLocale();


	char c;
	short h;
	int d;
	int32_t l;
	int64_t ll;
	ssize_t z;
	intmax_t j;
	ptrdiff_t t;
	fmt("a%hhn", &c); EXPECT(c == 1);
	fmt("ab%hn", &h); EXPECT(h == 2);
	fmt("abc%n", &d); EXPECT(d == 3);
	fmt("abcd%ln", &l); EXPECT(l == 4);
	fmt("abcde%lln", &ll); EXPECT(ll == 5);
	fmt("abcdef%zn", &z); EXPECT(z == 6);
	fmt("abcdefg%jn", &j); EXPECT(j == 7);
	fmt("abcdefgh%tn", &t); EXPECT(t == 8);
}

TEST_DECL(divergend)
{
	do_vprintf = sb_vxprintf;
	EXPECT_STR(fmt("|%5s|%#7s|", "Äbc", "Äbc"), "| Äbc| \"Äbc\"|");
	EXPECT_STR(fmt("|%-5s|%-#7s|", "Äbc", "Äbc"), "|Äbc |\"Äbc\" |");
	EXPECT_STR(fmt("|%-5s|%-#5s|", "Äbc", "Äbc"), "|Äbc |\"Äbc\"|");
	EXPECT_STR(fmt("|%3.2s|%#3.2s|", "äöü", "äöü"), "| ä|\"ä\"|");
	EXPECT_STR(fmt("|%3.-2s|%#3.-2s|", "äöü", "äöü"), "| ü|\"ü\"|");
	do_vprintf = sb_vmbprintf;
	EXPECT_STR(fmt("|%5s|%#7s|", "Äbc", "Äbc"), "|  Äbc|  \"Äbc\"|");
	EXPECT_STR(fmt("|%-5s|%-#7s|", "Äbc", "Äbc"), "|Äbc  |\"Äbc\"  |");
	EXPECT_STR(fmt("|%-4s|%-#4s|", "Äbc", "Äbc"), "|Äbc |\"Äbc\"|");
	EXPECT_STR(fmt("|%3.2s|%#3.2s|", "äöü", "äöü"), "| äö|\"äö\"|");
	EXPECT_STR(fmt("|%3.-2s|%#3.-2s|", "äöü", "äöü"), "| öü|\"öü\"|");
}

TEST_DECL(fmt_bool)
{
	StrBuf *sb = sb_create(2);
	fmt_bool(sb, getfmt("b"), 0);
	sb_putc(',', sb);
	fmt_bool(sb, getfmt("b"), 1);
	sb_putc(',', sb);
	fmt_bool(sb, getfmt("d"), 0);
	sb_putc(',', sb);
	fmt_bool(sb, getfmt("d"), 1);
	EXPECT_STR(sb_nul(sb), "0,1,false,true");
	sb_destroy(sb);
}

TEST_SUITE
{
	TEST_RUN(getc);
	TEST_RUN(sb_printf);
	do_vprintf = sb_vprintf;
	TEST_RUN(fmt);
	TEST_RUN(sb_xprintf);
	do_vprintf = sb_vxprintf;
	TEST_RUN(fmt);
	TEST_RUN(xfmt);
	TEST_RUN(sb_mbprintf);
	do_vprintf = sb_vmbprintf;
	TEST_RUN(fmt);
	TEST_RUN(xfmt);
	TEST_RUN(fmt_bool);
	TEST_RUN(divergend);
}
