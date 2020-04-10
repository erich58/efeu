#include <EFEU/UnitTest.h>
#include <EFEU/io.h>

#define	EXPECT_BUF(a, b) \
	UnitTest_strcheck(__FILE__, __LINE__, "==", sb_nul(a), b, # a, # b)

#define	EXPECT_DATA(a, b) \
	UnitTest_strcheck(__FILE__, __LINE__, "==", DATA(a), b, # a, # b)

static IO *IN(char *def)
{
	static IO *io = NULL;
	io_close(io);
	io = def ? io_string(def, NULL) : NULL;
	return io;
}

static char *DATA(char *def)
{
	static char *buf = NULL;
	memfree(buf);
	buf = def ? def : NULL;
	return buf;
}

#define	EXPECT_GETARG(def, beg, end, res) \
	EXPECT_DATA(io_getarg(IN(def), beg, end), res)

TEST_DECL(getarg)
{
	EXPECT_GETARG("[a(b)c]", '[', ']', "a(b)c");
	EXPECT_GETARG("[a(bc]", '[', ']', "a(bc]");
	EXPECT_GETARG("[a(b{c)d]", '[', ']', "a(b{c)d]");
	EXPECT_GETARG("|a[bc|d]|ef", '|', '|', "a[bc|d]");
	EXPECT_GETARG("x|a", '|', '|', NULL);
}

TEST_SUITE
{
	TEST_RUN(getarg);
	// Speicherfreigabe
	IN(NULL);
	DATA(NULL);
}
