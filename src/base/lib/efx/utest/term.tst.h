/*	Test auf Termauswertung mit Freigabe
*/

#ifndef	_utest_term_tst_h
#define	_utest_term_tst_h	1

#include <EFEU/UnitTest.h>
#include <EFEU/object.h>
#include <EFEU/memalloc.h>

static char *utest_term(char *def)
{
	static char *buf = NULL;
	memfree(buf);
	buf = def ? Obj2str(strterm(def)) : NULL;
	return buf;
}

#define	EXPECT_TERM(a, b) \
	UnitTest_strcheck(__FILE__, __LINE__, "==", \
		utest_term(a), b, "term(" # a ")", # b)

#define	MATCH_TERM(a, b) \
	UnitTest_strcheck(__FILE__, __LINE__, "~", \
		utest_term(a), b, "term(" # a ")", # b)

#define	PRINT_TERM(s) \
	UnitTest_strinfo(__FILE__, __LINE__, "INFO", \
		utest_term(s), "term(" # s ")")

#define	CLEANUP_TERM \
	utest_term(NULL)

#endif
