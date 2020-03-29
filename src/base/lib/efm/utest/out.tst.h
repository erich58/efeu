#ifndef	_utest_out_tst_h
#define	_utest_out_tst_h	1

#include <EFEU/UnitTest.h>
#include <EFEU/strbuf.h>
#include <EFEU/io.h>

static STRBUF(utest_data, 256);

static IO *OUT(void)
{
	static IO *utest_out = NULL;
	sb_clean(&utest_data);

	if	(!utest_out)
		utest_out = io_strbuf(&utest_data);

	return utest_out;
}

#define	DATA() sb_nul(&utest_data)

#endif
