/*	Interpreterauswertung
*/

#ifndef	_utest_eval_tst_h
#define	_utest_eval_tst_h	1

#include <EFEU/UnitTest.h>
#include <EFEU/object.h>
#include <EFEU/memalloc.h>
#include "utest/in.tst.h"
#include "utest/out.tst.h"

static const char *utest_eval(const char *def)
{
	iocpy_eval(IN(def), OUT(), 0, NULL, 0);
	return utest_out();
}

#define	EXPECT_EVAL(a, b) \
	UnitTest_strcheck(__FILE__, __LINE__, "==", \
		utest_eval(a), b, "eval(" # a ")", # b)

#define	MATCH_EVAL(a, b) \
	UnitTest_strcheck(__FILE__, __LINE__, "~", \
		utest_eval(a), b, "eval(" # a ")", # b)

#define	PRINT_EVAL(s) \
	UnitTest_strinfo(__FILE__, __LINE__, "INFO", \
		utest_eval(s), "eval(" # s ")")

#define	CLEANUP_EVAL \
	CLEANUP_IN, CLEANUP_OUT

#endif
