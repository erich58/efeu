/*	Test auf dynamisch generierten String mit Freigabe
*/

#ifndef	_utest_mstr_tst_h
#define	_utest_mstr_tst_h	1

#include <EFEU/UnitTest.h>
#include <EFEU/memalloc.h>

static char *utest_mstr(char *def)
{
	static char *buf = NULL;
	memfree(buf);
	buf = def ? def : NULL;
	return buf;
}

#define	EXPECT_MSTR(a, b) \
	UnitTest_strcheck(__FILE__, __LINE__, "==", utest_mstr(a), b, # a, # b)

#define	MATCH_MSTR(a, b) \
	UnitTest_strcheck(__FILE__, __LINE__, "~", utest_mstr(a), b, # a, # b)

#define	PRINT_MSTR(s) \
	UnitTest_strinfo(__FILE__, __LINE__, "INFO", utest_mstr(s), # s)

#define	CLEANUP_MSTR \
	utest_mstr(NULL)

#endif
