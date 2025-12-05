#include <EFEU/UnitTest.h>
#include "TimeRange.h"
#include <EFEU/locale.h>
#include <EFEU/utest/in.tst.h>
#include <EFEU/utest/out.tst.h>

static const char *trange(const char *def)
{
	static VECBUF(buf, 32, sizeof(TimeRange));

	if	(!def)
	{
		vb_free(&buf);
		return NULL;
	}

	vb_clear(&buf, NULL);
	int type = strTimeRange(&buf, def);
	IO *out = OUT();
	io_printf(out, "%c[%d]\n", type ? type : '*', buf.used);
	PrintTimeRange(out, buf.data, buf.used);
	return utest_out();
}

#define	EXPECT_RANGE(a, b) \
	UnitTest_strcheck(__FILE__, __LINE__, "==", \
		trange(a), b, "TimeRange(" # a ")", # b)

#define	CLEANUP_RANGE	trange(NULL)

TEST_SUITE
{
	SetProgName("ID");
	SetLocale(LOC_DATE, "C");
	SetApplPath(UnitTest_tmp(NULL));
	TEST_FILE("mdmat.msg",
		"305	$!: time range $0: unexpected character $1.\n"
	);
	
	EXPECT_INT(strTimeRange(NULL, NULL), 0);
	EXPECT_INT(strTimeRange(NULL, ""), 'j');
	EXPECT_INT(strTimeRange(NULL, "#comment line\n"), 'j');
	EXPECT_INT(strTimeRange(NULL, "2000"), 'j');
	EXPECT_INT(strTimeRange(NULL, "2000/1"), 'q');
	EXPECT_INT(strTimeRange(NULL, "2000-1"), 'm');
	EXPECT_INT(strTimeRange(NULL, "2000-1-1"), 't');
	EXPECT_INT(strTimeRange(NULL, "2000-1-1 12"), 'H');
	EXPECT_INT(strTimeRange(NULL, "2000-1-1 12:30"), 'M');
	EXPECT_INT(strTimeRange(NULL, "2000-1-1 12:30:10"), 'S');
	EXPECT_INT(strTimeRange(NULL, "1.2000"), 'm');
	EXPECT_INT(strTimeRange(NULL, "1.1.2000"), 't');
	EXPECT_INT(strTimeRange(NULL, "1.1.2000 12"), 'H');
	EXPECT_INT(strTimeRange(NULL, "1.1.2000 12:30"), 'M');
	EXPECT_INT(strTimeRange(NULL, "1.1.2000 12:30:10"), 'S');
	EXPECT_INT(strTimeRange(NULL, "12:30"), 'M');
	EXPECT_INT(strTimeRange(NULL, "12:30:10"), 'S');

	UnitTest_stderr("stderr");
	EXPECT_INT(IOTimeRange(NULL, IN("abc")), 'j');
	UnitTest_stderr(NULL);
	EXPECT_CONTENT("stderr",
		"ERR ID: time range \"abc\":1: unexpected character 'a'.\n");

	EXPECT_RANGE("2000",
		"j[1]\n"
		"2000        \t01.01.2000 - 31.12.2000\t01.07.2000  366\n");
	EXPECT_RANGE("2000/2",
		"q[1]\n"
		"2000/2      \t01.04.2000 - 30.06.2000\t16.05.2000   91\n");
	EXPECT_RANGE("2000-01",
		"m[1]\n"
		"2000-01     \t01.01.2000 - 31.01.2000\t16.01.2000   31\n");
	EXPECT_RANGE("2000-01-10",
		"t[1]\n"
		"2000-01-10  \t10.01.2000 - 10.01.2000\t10.01.2000    1\n");
	EXPECT_RANGE("10.1.2000",
		"t[1]\n"
		"2000-01-10  \t10.01.2000 - 10.01.2000\t10.01.2000    1\n");
	EXPECT_RANGE("2000-01-10 12",
		"H[1]\n"
		"2000-01-10  \t10.01.2000 - 10.01.2000\t10.01.2000    1\n");
	EXPECT_RANGE("12:30:00",
		"S[1]\n"
		"0000-00-00  \t00.00.0000 - 00.00.0000\t00.00.0000    1\n");

	EXPECT_RANGE("2000-01-10 12:00-2000-01-10 15:00[%b %H:%M]",
		"M[1]\n"
		"Jan 12:00   \t10.01.2000 - 10.01.2000\t10.01.2000    1\n");
	EXPECT_RANGE("2000-04,- 2000-06",
		"m[1]\n"
		"2000-04-2000-06\t01.04.2000 - 30.06.2000\t16.05.2000   91\n");
	EXPECT_RANGE("2000-01..2000-03",
		"m[3]\n"
		"2000-01     \t01.01.2000 - 31.01.2000\t16.01.2000   31\n"
		"2000-02     \t01.02.2000 - 29.02.2000\t15.02.2000   29\n"
		"2000-03     \t01.03.2000 - 31.03.2000\t16.03.2000   31\n"
	);

	TEST_FILE("data",
		"2000-01-01;\n"
		"2000-02;\n"
	);
	EXPECT_RANGE(UnitTest_tpl("\\@@/data"),
		"t[2]\n"
		"2000-01-01  \t01.01.2000 - 01.01.2000\t01.01.2000    1\n"
		"2000-02     \t01.02.2000 - 29.02.2000\t15.02.2000   29\n"
	);

	CLEANUP_RANGE;
	CLEANUP_IN;
	CLEANUP_OUT;
}
