/*	Polynomdarstellung
*/

#ifndef _utest_pnom_tst_h
#define _utest_pnom_tst_h	1

#include <EFEU/UnitTest.h>
#include <Math/pnom.h>
#include "utest/out.tst.h"

static Polynom *utest_pnom(const char *file, int line,
	Polynom *pn, const char *s, const char *expr, const char *xs)
{
	if	(pn)
	{
		pnprint(OUT(), pn, "%.2f");
		UnitTest_strcheck(file, line, "==", utest_out(), s, expr, xs);
	}
	else if	(s)
	{
		pn = str2pnom(s);
		pnprint(OUT(), pn, "%.2f");
		UnitTest_strinfo(file, line, "INFO", utest_out(), expr);
	}

	return pn;
}

#define	TEST_PNOM(name, cont)	\
Polynom *name = utest_pnom(__FILE__, __LINE__, NULL, cont, # name, NULL)

#define	EXPECT_PNOM(expr, cont)	\
rd_deref(utest_pnom(__FILE__, __LINE__, expr, cont, # expr, # cont))

#define CLEANUP_PNOM	CLEANUP_OUT

#endif // _utest_pnom_tst_h
