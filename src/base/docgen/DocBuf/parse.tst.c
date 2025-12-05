#include <EFEU/UnitTest.h>
#include <EFEU/mstring.h>
#include "DocBuf.h"
#include <EFEU/utest/in.tst.h>

TEST_SUITE
{
	StrBuf *buf;
	EXPECT_PTR(buf = parse_open());
	EXPECT_STR(parse_close(buf), NULL);
	EXPECT_PTR(buf = parse_open());
	EXPECT_INT(sb_puts("Text", buf), 4);
	EXPECT_STR(parse_close(buf), "Text");

	IO *in;
	TEST_EXPR(in = IN("abc."));
	EXPECT_STR(parse_name(in, 'X'), "Xabc");
	EXPECT_INT(io_peek(in), '.');
	TEST_EXPR(in = IN("abc}\n"));
	EXPECT_STR(parse_block(in, '{', '}', 0), "{abc}");
	EXPECT_INT(io_peek(in), '\n');
	CLEANUP_IN;
}
