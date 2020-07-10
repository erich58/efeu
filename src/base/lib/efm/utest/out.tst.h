#ifndef	_utest_out_tst_h
#define	_utest_out_tst_h	1

#include <EFEU/UnitTest.h>
#include <EFEU/strbuf.h>
#include <EFEU/io.h>

static STRBUF(utest_out_buf, 256);
static IO *utest_out_io = NULL;

static void utest_out_clean(void)
{
	io_close(utest_out_io);
	utest_out_io = NULL;
	sb_free(&utest_out_buf);
}

static IO *OUT(void)
{
	if	(!utest_out_io)
		utest_out_io = io_strbuf(&utest_out_buf);

	return utest_out_io;
}

static const char *utest_out(void)
{
	const char *p = sb_nul(&utest_out_buf);
	sb_trunc(&utest_out_buf);
	return p;
}

#define	EXPECT_OUT(s)	UnitTest_strcheck(__FILE__, __LINE__, \
	"==", utest_out(), s, "output()", # s)

#define	MATCH_OUT(s)	UnitTest_strcheck(__FILE__, __LINE__, \
	"~", utest_out(), s, "output()", # s)

#define	PRINT_OUT()	UnitTest_strinfo(__FILE__, __LINE__, \
	"INFO", utest_out(), "output()")

#define	CLEANUP_OUT	utest_out_clean()


#endif
